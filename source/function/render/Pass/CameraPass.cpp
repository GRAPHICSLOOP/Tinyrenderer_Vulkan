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
    colorAttachmentDesc.setFormat(mFrameBuffer.mAttachments[EAttachmentType::color].mFormat);
    colorAttachmentDesc.setSamples(vk::SampleCountFlagBits::e1);
    colorAttachmentDesc.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachmentDesc.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorAttachmentDesc.setInitialLayout(vk::ImageLayout::eUndefined);
    colorAttachmentDesc.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    colorAttachmentDesc.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachmentDesc.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[1];
    depthAttachmentDesc.setFormat(mFrameBuffer.mAttachments[EAttachmentType::depth].mFormat);
    depthAttachmentDesc.setSamples(vk::SampleCountFlagBits::e1);
    depthAttachmentDesc.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthAttachmentDesc.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachmentDesc.setInitialLayout(vk::ImageLayout::eUndefined);
    depthAttachmentDesc.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    depthAttachmentDesc.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachmentDesc.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

    vk::AttachmentReference attachmentRef[2];
    attachmentRef[0].setAttachment(0);
    attachmentRef[0].setLayout(vk::ImageLayout::eColorAttachmentOptimal); // 过程中的布局,这也是为什么subpass可以多个
    attachmentRef[1].setAttachment(1);
    attachmentRef[1].setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpassDesc;
    subpassDesc.setColorAttachments(attachmentRef[0]);
    subpassDesc.setPDepthStencilAttachment(&attachmentRef[1]);
    subpassDesc.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency subpassDependency;
    subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL); // 因为我们不依赖任何subpass
    subpassDependency.setDstSubpass(0);
    subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eTopOfPipe);
    subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
    subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

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
