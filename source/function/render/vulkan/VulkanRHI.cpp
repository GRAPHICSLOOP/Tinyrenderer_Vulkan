#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "core/base/macro.h"

tiny::VulkanRHI::~VulkanRHI()
{
    for (uint32_t i = 0; i < mSwapchainSupportDetails.mImageCount; i++)
    {
        mDevice.destroyImageView(mSwapchainImageViews[i]);
    }
    mDevice.destroySwapchainKHR(mSwapchain);
    mDevice.destroyDescriptorPool(mDescriptorPool);
    mDevice.destroySemaphore(mImageAvailableSemaphore);
    mDevice.destroySemaphore(mRenderFinishedSemaphore);
    mDevice.destroyFence(mFence);
    mDevice.destroyCommandPool(mCommandPool);
    mDevice.destroy();
    mInstance.destroySurfaceKHR(mSurfaceKHR);
    mInstance.destroy();
}

void tiny::VulkanRHI::initialize(const VulkanConfigParams& params)
{
    mWindowSystem = params.mWindowSystem;

    createInstance();

    //initializeDebugMessenger();

    createWindowSurface();

    initializePhysicalDevice();

    createLogicalDevice();

    createCommandPool();

    createCommandBuffers();

    createDescriptorPool();

    createSyncPrimitives();

    createSwapchain();

    createSwapchainImageViews();

    createFramebufferImageAndView();
}

vk::CommandBuffer tiny::VulkanRHI::beginSingleTimeBuffer()
{
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    vk::CommandBuffer buffer = mDevice.allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    buffer.begin(beginInfo);
    return buffer;
}

void tiny::VulkanRHI::endSingleTimeBuffer(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::Result result = mGraphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
    mGraphicsQueue.waitIdle();

    mDevice.freeCommandBuffers(mCommandPool, commandBuffer);
}

void tiny::VulkanRHI::prepareBeforePass()
{
    mDevice.resetCommandPool(mCommandPool);

    // 获取imageindex
    vk::Result result = mDevice.acquireNextImageKHR(
        mSwapchain,
        UINT64_MAX,
        mImageAvailableSemaphore,
        VK_NULL_HANDLE,
        &nextImageIndex);

    // 记录命令
    vk::CommandBufferBeginInfo info;
    info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit; // default 0
    info.pInheritanceInfo = nullptr;
    mCommandBuffer.begin(info);
}

void tiny::VulkanRHI::submitRendering()
{
    mCommandBuffer.end();

    // 提交命令buffer
    vk::SubmitInfo submitInfo;
    std::array<vk::PipelineStageFlags, 1> waiteStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &mImageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waiteStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &mRenderFinishedSemaphore;

    if (mGraphicsQueue.submit(1, &submitInfo, mFence) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinishedSemaphore;

    presentInfo.pSwapchains = &mSwapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pImageIndices = &nextImageIndex;

    vk::Result result = mPresentQueue.presentKHR(presentInfo);
}

const uint32_t tiny::VulkanRHI::getNextImageIndex() const
{
    return nextImageIndex;
}

void tiny::VulkanRHI::createInstance()
{
    std::vector<const char*> extensions;
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> tempExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions = tempExtensions;
    }

    vk::InstanceCreateInfo info;
    info.ppEnabledExtensionNames = extensions.data();
    info.enabledExtensionCount = (uint32_t)extensions.size();
    if (enableValidationLayers)
    {
        info.ppEnabledLayerNames = mEnableLayerNames.data();
        info.enabledLayerCount = (uint32_t)mEnableLayerNames.size();
    }

    mInstance = vk::createInstance(info);
    CHECK_NULL(mInstance);
}

void tiny::VulkanRHI::createWindowSurface()
{
    glfwCreateWindowSurface(mInstance, mWindowSystem->getWindow(), nullptr, &mSurfaceKHR);
    CHECK_NULL(mSurfaceKHR);
}

void tiny::VulkanRHI::initializePhysicalDevice()
{
    mPhyDevice = mInstance.enumeratePhysicalDevices()[0];
    CHECK_NULL(mPhyDevice);
}

void tiny::VulkanRHI::createLogicalDevice()
{
    findQueueFamilyIndeices();
    std::vector<vk::DeviceQueueCreateInfo> queueInfos;
    if (mQueueFamilyIndices.graphicsIndices.value() == mQueueFamilyIndices.presentIndices.value())
    {
        float priorities = 1.f;
        queueInfos.resize(1);
        queueInfos[0].pQueuePriorities = &priorities;
        queueInfos[0].queueFamilyIndex = mQueueFamilyIndices.graphicsIndices.value();
        queueInfos[0].queueCount = 1;
    }
    else
    {
        float priorities = 1.f;
        queueInfos.resize(2);

        queueInfos[0].pQueuePriorities = &priorities;
        queueInfos[0].queueFamilyIndex = mQueueFamilyIndices.graphicsIndices.value();
        queueInfos[1].queueCount = 1;
        queueInfos[1].pQueuePriorities = &priorities;
        queueInfos[1].queueFamilyIndex = mQueueFamilyIndices.presentIndices.value();
        queueInfos[1].queueCount = 1;
    }
    std::vector<const char*> extensionNames = { "VK_KHR_swapchain" };

    vk::DeviceCreateInfo info;
    info.pQueueCreateInfos = queueInfos.data();
    info.queueCreateInfoCount = (uint32_t)queueInfos.size();
    info.ppEnabledExtensionNames = extensionNames.data();
    info.enabledExtensionCount = (uint32_t)extensionNames.size();

    mDevice = mPhyDevice.createDevice(info);
    CHECK_NULL(mDevice);

    mGraphicsQueue = mDevice.getQueue(mQueueFamilyIndices.graphicsIndices.value(), 0);
    mPresentQueue = mDevice.getQueue(mQueueFamilyIndices.presentIndices.value(), 0);
}

void tiny::VulkanRHI::createCommandPool()
{
    vk::CommandPoolCreateInfo info;
    info.queueFamilyIndex = mQueueFamilyIndices.graphicsIndices.value();

    mCommandPool  =  mDevice.createCommandPool(info);
    CHECK_NULL(mCommandPool);
}

void tiny::VulkanRHI::createCommandBuffers()
{
    vk::CommandBufferAllocateInfo info;
    info.commandBufferCount = 1;
    info.commandPool = mCommandPool;
    info.level = vk::CommandBufferLevel::ePrimary;

    mCommandBuffer = mDevice.allocateCommandBuffers(info)[0];
    CHECK_NULL(mCommandBuffer);
}

void tiny::VulkanRHI::createDescriptorPool()
{
    std::array<vk::DescriptorPoolSize, 2> poolSize;
    poolSize[0].descriptorCount = 1;
    poolSize[0].type = vk::DescriptorType::eUniformBuffer;
    poolSize[1].descriptorCount = 1;
    poolSize[1].type = vk::DescriptorType::eCombinedImageSampler;

    vk::DescriptorPoolCreateInfo info;
    info.poolSizeCount = 2;
    info.pPoolSizes = poolSize.data();
    info.maxSets = 2; // 这里其实可以设置大一些，但是为了不浪费，我们就将它的大小刚好等于我们后面分配的DescriptorSet的大小
    info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

    mDescriptorPool = mDevice.createDescriptorPool(info);
    CHECK_NULL(mDescriptorPool);
}

void tiny::VulkanRHI::createSyncPrimitives()
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    mImageAvailableSemaphore = mDevice.createSemaphore(semaphoreInfo);
    CHECK_NULL(mImageAvailableSemaphore);
    mRenderFinishedSemaphore = mDevice.createSemaphore(semaphoreInfo);
    CHECK_NULL(mRenderFinishedSemaphore);
    mFence = mDevice.createFence(fenceInfo);
    CHECK_NULL(mFence);
}

void tiny::VulkanRHI::createSwapchain()
{
    findSwapchainSupport();

    vk::SwapchainCreateInfoKHR info;
    info.clipped = VK_TRUE;
    info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;// 不透明
    info.surface = mSurfaceKHR;
    info.presentMode = mSwapchainSupportDetails.mPresentMode;
    info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity; // 默认，其他模式需要根据显卡特性
    info.imageArrayLayers = 1;
    info.imageColorSpace = mSwapchainSupportDetails.mFormat.colorSpace;
    info.imageFormat = mSwapchainSupportDetails.mFormat.format;
    info.imageExtent = mSwapchainSupportDetails.mExtent2D;
    info.minImageCount = mSwapchainSupportDetails.mImageCount;
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    if (mQueueFamilyIndices.graphicsIndices.value() != mQueueFamilyIndices.presentIndices.value())
    {
        std::array<uint32_t, 2> indices = { mQueueFamilyIndices.graphicsIndices.value() ,mQueueFamilyIndices.presentIndices.value() };
        info.imageSharingMode = vk::SharingMode::eConcurrent;
        info.pQueueFamilyIndices = indices.data();
        info.queueFamilyIndexCount = (uint32_t)indices.size();
    }
    else
    {
        info.imageSharingMode = vk::SharingMode::eExclusive;
        info.pQueueFamilyIndices = &mQueueFamilyIndices.graphicsIndices.value();
        info.queueFamilyIndexCount = 1;
    }
    

    mSwapchain = mDevice.createSwapchainKHR(info);
    CHECK_NULL(mSwapchain);

    mSwapchainImages = mDevice.getSwapchainImagesKHR(mSwapchain);
}

void tiny::VulkanRHI::createSwapchainImageViews()
{
    for (uint32_t i = 0; i < mSwapchainSupportDetails.mImageCount; i++)
    {
        vk::ImageView imageView = VulkanUtil::createImageView(
            vk::ImageAspectFlagBits::eColor, 
            mSwapchainSupportDetails.mFormat.format, 
            mSwapchainImages[i]);
        CHECK_NULL(imageView);

        mSwapchainImageViews.push_back(imageView);
    }
}

void tiny::VulkanRHI::createFramebufferImageAndView()
{

}

void tiny::VulkanRHI::findQueueFamilyIndeices()
{
    auto properties = mPhyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags | vk::QueueFlagBits::eGraphics)
        {
            mQueueFamilyIndices.graphicsIndices = i;
        }
        if (mPhyDevice.getSurfaceSupportKHR(i, mSurfaceKHR))
        {
            mQueueFamilyIndices.presentIndices = i;
        }

        if (mQueueFamilyIndices.isComplete())
        {
            return;
        }
    }
}

void tiny::VulkanRHI::findSwapchainSupport()
{
    // capabilities
    mSwapchainSupportDetails.mCapabilities = mPhyDevice.getSurfaceCapabilitiesKHR(mSurfaceKHR);

    // formats
    mSwapchainSupportDetails.mFormat = chooseSwapchainSurfaceFormatFromDetails(mPhyDevice.getSurfaceFormatsKHR(mSurfaceKHR));

    // presentMode
    mSwapchainSupportDetails.mPresentMode = chooseSwapchainPresentModeFromDetails(mPhyDevice.getSurfacePresentModesKHR(mSurfaceKHR));

    // extend2D
    mSwapchainSupportDetails.mExtent2D = chooseSwapchainExtentFromDetails(mSwapchainSupportDetails.mCapabilities);

    mSwapchainSupportDetails.mImageCount = mSwapchainSupportDetails.mCapabilities.minImageCount;

    TINYLOG_INFO("swapchain numb : {}", mSwapchainSupportDetails.mImageCount);
}

vk::SurfaceFormatKHR tiny::VulkanRHI::chooseSwapchainSurfaceFormatFromDetails(std::vector<vk::SurfaceFormatKHR> format)
{
    for (const auto& surface_format : format)
    {
        // TODO: select the VK_FORMAT_B8G8R8A8_SRGB surface format,
        // there is no need to do gamma correction in the fragment shader
        if (surface_format.format == vk::Format::eB8G8R8A8Unorm &&
            surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }
    return format[0];
}

vk::PresentModeKHR tiny::VulkanRHI::chooseSwapchainPresentModeFromDetails(std::vector<vk::PresentModeKHR> mode)
{
    for (const auto& present_mode : mode)
    {
        if (vk::PresentModeKHR::eMailbox == present_mode)
        {
            return vk::PresentModeKHR::eMailbox;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D tiny::VulkanRHI::chooseSwapchainExtentFromDetails(vk::SurfaceCapabilitiesKHR cap)
{
    if (cap.currentExtent.width != UINT32_MAX)
    {
        return cap.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(mWindowSystem->getWindow(), &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width =
            std::clamp(actualExtent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

        return actualExtent;
    }
}
