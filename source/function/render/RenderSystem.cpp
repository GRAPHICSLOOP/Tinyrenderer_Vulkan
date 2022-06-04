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
