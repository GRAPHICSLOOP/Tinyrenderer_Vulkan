#include "CameraPass.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"

tiny::MainCameraPass::~MainCameraPass()
{
    mVulkanRHI->mDevice.destroyRenderPass(mRenderPass);
    std::vector<FrameBufferAttachment> attachments = mFrameBuffer.getAttachments();
    for (uint32_t i = 0; i < attachments.size(); i++)
    {
        mVulkanRHI->mDevice.destroyImage(attachments[i].mImage);
        mVulkanRHI->mDevice.destroyImageView(attachments[i].mImageView);
        mVulkanRHI->mDevice.freeMemory(attachments[i].mMemory);
    }
}

void tiny::MainCameraPass::initialize(PassConfigParams params)
{
    mVulkanRHI = params.mVulkanRHI;

    setupAttachments();
    setupRenderPass();
    setupDescriptorSetLayout();
    //setupPipelines();
    //setupDescriptorSet();
    //setupFramebufferDescriptorSet();
    //setupSwapchainFramebuffers();

}

void tiny::MainCameraPass::setupAttachments()
{
    FrameBufferAttachment colorFrameBufferAttachment;
    colorFrameBufferAttachment.mFormat = vk::Format::eR8G8B8A8Unorm;
    VulkanUtil::createImage(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width,
        mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height,
        colorFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        colorFrameBufferAttachment.mImage,
        colorFrameBufferAttachment.mMemory);
    colorFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        mVulkanRHI->mDevice,
        vk::ImageAspectFlagBits::eColor,
        colorFrameBufferAttachment.mFormat,
        colorFrameBufferAttachment.mImage);

    FrameBufferAttachment depthFrameBufferAttachment;
    depthFrameBufferAttachment.mFormat = vk::Format::eD24UnormS8Uint;
    VulkanUtil::createImage(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width,
        mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height,
        depthFrameBufferAttachment.mFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        depthFrameBufferAttachment.mImage,
        depthFrameBufferAttachment.mMemory);
    depthFrameBufferAttachment.mImageView = VulkanUtil::createImageView(
        mVulkanRHI->mDevice,
        vk::ImageAspectFlagBits::eDepth,
        depthFrameBufferAttachment.mFormat,
        depthFrameBufferAttachment.mImage);

    mFrameBuffer.mAttachments.insert(std::make_pair(EAttachmentType::color, colorFrameBufferAttachment));
    mFrameBuffer.mAttachments.insert(std::make_pair(EAttachmentType::depth, depthFrameBufferAttachment));
}

void tiny::MainCameraPass::setupRenderPass()
{
    std::array<vk::AttachmentDescription, 2> attachmentDesces;

    vk::AttachmentDescription& colorAttachmentDesc = attachmentDesces[0];
    colorAttachmentDesc.format = mFrameBuffer.mAttachments[EAttachmentType::color].mFormat;
    colorAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDesc.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    colorAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[1];
    depthAttachmentDesc.format = mFrameBuffer.mAttachments[EAttachmentType::depth].mFormat;
    depthAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    depthAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachmentDesc.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachmentDesc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depthAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentReference attachmentRef[2];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = vk::ImageLayout::eColorAttachmentOptimal; // 过程中的布局,这也是为什么subpass可以多个
    attachmentRef[1].attachment = 1;
    attachmentRef[1].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpassDesc;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &attachmentRef[0];
    subpassDesc.pDepthStencilAttachment = &attachmentRef[1];
    subpassDesc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    vk::SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL; // 因为我们不依赖任何subpass
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlagBits::eNone;
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo info;
    info.setAttachments(attachmentDesces);
    info.setDependencies(subpassDependency);
    info.setSubpasses(subpassDesc);

    mRenderPass = mVulkanRHI->mDevice.createRenderPass(info);
    CHECK_NULL(mRenderPass);
}

void tiny::MainCameraPass::setupDescriptorSetLayout()
{
}
