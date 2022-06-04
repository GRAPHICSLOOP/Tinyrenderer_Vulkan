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
    info.enabledExtensionCount = extensions.size();
    if (enableValidationLayers)
    {
        info.ppEnabledLayerNames = mEnableLayerNames.data();
        info.enabledLayerCount = mEnableLayerNames.size();
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
    info.enabledExtensionCount = extensionNames.size();

    mDevice = mPhyDevice.createDevice(info);
    CHECK_NULL(mDevice);
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
}

void tiny::VulkanRHI::createSyncPrimitives()
{
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
        vk::ImageView imageView = VulkanUtil::createImageView(mDevice, 
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
