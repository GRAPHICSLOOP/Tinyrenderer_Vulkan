#include "CameraPass.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"
#include <glm/gtc/matrix_transform.hpp>

tiny::MainCameraPass::~MainCameraPass()
{
    for (uint32_t i = 0; i < mFrame.mFramebuffer.size(); i++)
    {
        mVulkanRHI->mDevice.destroyFramebuffer(mFrame.mFramebuffer[i]);
    }
    mVulkanRHI->mDevice.destroyPipelineLayout(mPipelineLayout);
    mVulkanRHI->mDevice.destroyPipeline(mPipeline);
    mVulkanRHI->mDevice.destroyRenderPass(mFrame.mRenderPass);
    mVulkanRHI->mDevice.destroyDescriptorSetLayout(mDescSetLayout);
    std::vector<FrameBufferAttachment> attachments = mFrame.getAttachments();
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
    mRenderResource = params.mRenderResource;

    setupAttachments();
    setupRenderPass();
    setupDescriptorSetLayout();
    setupPipelines();
    setupDescriptorSet();
    //setupFramebufferDescriptorSet();
    setupSwapchainFramebuffers();

}

void tiny::MainCameraPass::drawPass()
{
    TempUpdateUniformBuffer(0.01f);

    vk::RenderPassBeginInfo passBegineInfo;
    passBegineInfo.renderPass = mFrame.mRenderPass;
    passBegineInfo.framebuffer = mFrame.mFramebuffer[mVulkanRHI->getNextImageIndex()];
    vk::Rect2D area;
    area.offset = 0;
    area.extent = mVulkanRHI->mSwapchainSupportDetails.mExtent2D;
    passBegineInfo.setRenderArea(area);

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = std::array<float, 4>{0, 0, 0, 1.f};
    clearValues[1].depthStencil = 1.f;
    passBegineInfo.pClearValues = clearValues.data();
    passBegineInfo.clearValueCount = 2;

    mVulkanRHI->mCommandBuffer.beginRenderPass(passBegineInfo, vk::SubpassContents::eInline);
        mVulkanRHI->mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
        vk::DeviceSize offset = 0;
        mVulkanRHI->mCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, &mDescriptorSets[0], 0, nullptr);
        for (const auto& resource : mRenderResource->mMeshBufferResources)
        {
            mVulkanRHI->mCommandBuffer.bindVertexBuffers(0, 1, &resource.mVertexBuffer, &offset);
            mVulkanRHI->mCommandBuffer.bindIndexBuffer(resource.mIndexBuffer, offset, vk::IndexType::eUint32);
            mVulkanRHI->mCommandBuffer.drawIndexed(resource.mVertexCount, 1, 0, 0, 0);
        }
    mVulkanRHI->mCommandBuffer.endRenderPass();
}

void tiny::MainCameraPass::setupAttachments()
{
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

    mFrame.mAttachments.insert(std::make_pair(ATTACHMENT_TYPE::TYPE_DEPTH, depthFrameBufferAttachment));

    VulkanUtil::transitionImageLayout(
        mVulkanRHI.get(),
        depthFrameBufferAttachment.mImage,
        depthFrameBufferAttachment.mFormat,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal,
        1);
}

void tiny::MainCameraPass::setupRenderPass()
{
    std::array<vk::AttachmentDescription, 2> attachmentDesces;

    vk::AttachmentDescription& colorAttachmentDesc = attachmentDesces[0];
    colorAttachmentDesc.format = mVulkanRHI->mSwapchainSupportDetails.mFormat.format;
    colorAttachmentDesc.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDesc.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    colorAttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::AttachmentDescription& depthAttachmentDesc = attachmentDesces[1];
    depthAttachmentDesc.format = mFrame.mAttachments[ATTACHMENT_TYPE::TYPE_DEPTH].mFormat;
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

    mFrame.mRenderPass = mVulkanRHI->mDevice.createRenderPass(info);
    CHECK_NULL(mFrame.mRenderPass);
}

void tiny::MainCameraPass::setupDescriptorSetLayout()
{
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings;
    vk::DescriptorSetLayoutBinding& uniformBinding = bindings[0];
    uniformBinding.binding = 0;
    uniformBinding.descriptorCount = 1;
    uniformBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uniformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutBinding& sampleBinding = bindings[1];
    sampleBinding.binding = 1;
    sampleBinding.descriptorCount = 1;
    sampleBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    sampleBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

    vk::DescriptorSetLayoutCreateInfo info;
    info.bindingCount = (uint32_t)bindings.size();
    info.pBindings = bindings.data();

    mDescSetLayout = mVulkanRHI->mDevice.createDescriptorSetLayout(info);
    CHECK_NULL(mDescSetLayout);
}

void tiny::MainCameraPass::setupPipelines()
{
    // vertex Descriptions
    vk::PipelineVertexInputStateCreateInfo vertexInfo;
    vertexInfo.vertexAttributeDescriptionCount = 3;
    vertexInfo.vertexBindingDescriptionCount = 1;
    vertexInfo.pVertexAttributeDescriptions = Vertex::getAttributeDescription().data();
    vertexInfo.pVertexBindingDescriptions = Vertex::getBindingDescription().data();

    // input assembler
    vk::PipelineInputAssemblyStateCreateInfo inputInfo;
    inputInfo.primitiveRestartEnable = false;
    inputInfo.topology = vk::PrimitiveTopology::eTriangleList;

    // shaderStatus
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStatus;
    vk::PipelineShaderStageCreateInfo& vertexShader = shaderStatus[0];
    vk::ShaderModule vertShaderModule = VulkanUtil::loadShaderModuleFromFile(mVulkanRHI->mDevice, "shaders/vert.spv");
    vk::ShaderModule fragShaderModule = VulkanUtil::loadShaderModuleFromFile(mVulkanRHI->mDevice, "shaders/frag.spv");
    vertexShader.module = vertShaderModule;
    vertexShader.pName = "main";
    vertexShader.stage = vk::ShaderStageFlagBits::eVertex;
    vk::PipelineShaderStageCreateInfo& fragShader = shaderStatus[1];
    fragShader.module = fragShaderModule;
    fragShader.pName = "main";
    fragShader.stage = vk::ShaderStageFlagBits::eFragment;

    // viewport
    vk::PipelineViewportStateCreateInfo viewportInfo;
    vk::Rect2D scissors;
    scissors.offset = 0;
    scissors.extent = mVulkanRHI->mSwapchainSupportDetails.mExtent2D;
    vk::Viewport viewport;
    viewport.height = (float)mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height;
    viewport.width = (float)mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width;
    viewport.maxDepth = 1.f;
    viewport.minDepth = 0.f;
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewportInfo.pScissors = &scissors;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.viewportCount = 1;
    
    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterInfo;
    rasterInfo.depthBiasEnable = false;
    rasterInfo.depthClampEnable = false;
    rasterInfo.rasterizerDiscardEnable = false;
    rasterInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterInfo.lineWidth = 1.f;
    rasterInfo.polygonMode = vk::PolygonMode::eFill;
    
    // multi sample
    vk::PipelineMultisampleStateCreateInfo multisampleInfo;
    multisampleInfo.alphaToCoverageEnable = false;
    multisampleInfo.alphaToOneEnable = false;
    multisampleInfo.sampleShadingEnable = false;
    
    // deptp/mask test
    vk::PipelineDepthStencilStateCreateInfo depthInfo;
    depthInfo.stencilTestEnable = false;
    depthInfo.depthTestEnable = true;
    depthInfo.depthWriteEnable = true;
    depthInfo.depthCompareOp = vk::CompareOp::eLess;
    
    // colorBlending
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.blendEnable = false;
    colorBlendAttachmentState.colorWriteMask = 
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo blendInfo;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = &colorBlendAttachmentState;
    blendInfo.blendConstants[0] = 0.f;
    blendInfo.blendConstants[1] = 0.f;
    blendInfo.blendConstants[2] = 0.f;
    blendInfo.blendConstants[3] = 0.f;
    blendInfo.logicOpEnable = false;
    blendInfo.logicOp = vk::LogicOp::eCopy;

    // dynamic state
    
    // pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescSetLayout;
    mPipelineLayout = mVulkanRHI->mDevice.createPipelineLayout(pipelineLayoutInfo);
    
    // pipeline RenderPass

    vk::GraphicsPipelineCreateInfo info;
    info.pVertexInputState = &vertexInfo;
    info.pInputAssemblyState = &inputInfo;
    info.stageCount = (uint32_t)shaderStatus.size();
    info.pStages = shaderStatus.data();
    info.pViewportState = &viewportInfo;
    info.pRasterizationState = &rasterInfo;
    info.pMultisampleState = &multisampleInfo;
    info.pDepthStencilState = &depthInfo;
    info.pColorBlendState = &blendInfo;
    info.pDynamicState = nullptr;
    info.layout = mPipelineLayout;
    info.renderPass = mFrame.mRenderPass;
    mPipeline = mVulkanRHI->mDevice.createGraphicsPipeline(VK_NULL_HANDLE,info).value;
    CHECK_NULL(mPipeline);

    mVulkanRHI->mDevice.destroyShaderModule(vertShaderModule);
    mVulkanRHI->mDevice.destroyShaderModule(fragShaderModule);
}

void tiny::MainCameraPass::setupDescriptorSet()                     
{
    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = mVulkanRHI->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &mDescSetLayout;
    mDescriptorSets = mVulkanRHI->mDevice.allocateDescriptorSets(info);

    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = mRenderResource->mTransfromResource.mBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(tiny::TransfromUniform);

    vk::DescriptorImageInfo imageInfo;
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.imageView = mRenderResource->mTextureResource.mImageView;
    imageInfo.sampler = mRenderResource->mSampleResource.mTextureSampler;

    // 更新描述符
    std::array<vk::WriteDescriptorSet, 2> writeSet;
    writeSet[0].dstArrayElement = 0;
    writeSet[0].dstBinding = 0;
    writeSet[0].dstSet = mDescriptorSets[0];
    writeSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writeSet[0].descriptorCount = 1;
    writeSet[0].pBufferInfo = &bufferInfo;
    writeSet[1].dstArrayElement = 0;
    writeSet[1].dstBinding = 1;
    writeSet[1].dstSet = mDescriptorSets[0];
    writeSet[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writeSet[1].descriptorCount = 1;
    writeSet[1].pImageInfo = &imageInfo;

    mVulkanRHI->mDevice.updateDescriptorSets(writeSet, nullptr);
}

void tiny::MainCameraPass::setupSwapchainFramebuffers()
{
    mFrame.mFramebuffer.resize(mVulkanRHI->mSwapchainSupportDetails.mImageCount);

    for (uint32_t i = 0; i < mVulkanRHI->mSwapchainSupportDetails.mImageCount; i++)
    {
        std::array<vk::ImageView, 2> attachments = { 
            mVulkanRHI->mSwapchainImageViews[i],
            mFrame.mAttachments[ATTACHMENT_TYPE::TYPE_DEPTH].mImageView
        };

        vk::FramebufferCreateInfo info;
        info.renderPass = mFrame.mRenderPass;
        info.pAttachments = attachments.data();
        info.attachmentCount = (uint32_t)attachments.size();
        info.height = mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height;
        info.width = mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width;
        info.layers = 1;

        mFrame.mFramebuffer[i] = mVulkanRHI->mDevice.createFramebuffer(info);
        CHECK_NULL(mFrame.mFramebuffer[i]);
    }
}

void tiny::MainCameraPass::TempUpdateUniformBuffer(float deltaTime)
{
    TransfromUniform ubo;
    ubo.mModel = glm::rotate(glm::mat4(1.f), deltaTime * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    ubo.mView = glm::lookAt(glm::vec3(1.f), glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f));
    ubo.mProj = glm::perspective(glm::radians(45.f), mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width / (float)mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height, 0.1f, 10.f);
    ubo.mProj[1][1] *= -1;

    vk::DeviceSize deviceSize = sizeof(ubo);
    void* data;
    vkMapMemory(mVulkanRHI->mDevice,mRenderResource->mTransfromResource.mMemory, 0, deviceSize, 0, &data);
    memcpy(data, &ubo, deviceSize);
    vkUnmapMemory(mVulkanRHI->mDevice, mRenderResource->mTransfromResource.mMemory);
}
