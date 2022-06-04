#include "RenderSystem.h"
#include "function/global/GlobalContext.h"

void tiny::RenderSystem::initialize()
{
	VulkanConfigParams vulkanParams;
	vulkanParams.mWindowSystem = tiny::gRuntimeGlobalContext.mWindowSystem;
	mVulkanRHI = std::make_shared<VulkanRHI>();
	mVulkanRHI->initialize(vulkanParams);

	RenderResourceConfigParams resourceParams;
	resourceParams.mVulkanRHI = mVulkanRHI;
	mRenderResource = std::make_shared<RenderResource>();
	mRenderResource->initialize(resourceParams);

	RenderPipelineConfigParams PipelineParams;
	PipelineParams.mVulkanRHI = mVulkanRHI;
	PipelineParams.mRenderResource = mRenderResource;
	mRenderPipeline = std::make_shared<RenderPipeline>();
	mRenderPipeline->initialize(PipelineParams);
}

void tiny::RenderSystem::tick(float deltaTime)
{
	// 等待上一帧渲染
	mVulkanRHI->mDevice.waitForFences(1, &mVulkanRHI->mFence, VK_TRUE, UINT64_MAX);
	mVulkanRHI->mDevice.resetFences(1, &mVulkanRHI->mFence);

	mVulkanRHI->prepareBeforePass();
	
	mRenderPipeline->draw(deltaTime);

	mVulkanRHI->submitRendering();
}
