#include "RenderSystem.h"
#include "function/global/GlobalContext.h"

void tiny::RenderSystem::initialize()
{
	VulkanConfigParams vulkanParams;
	vulkanParams.mWindowSystem = tiny::gRuntimeGlobalContext.mWindowSystem;

	mVulkanRHI = std::make_shared<VulkanRHI>();
	mVulkanRHI->initialize(vulkanParams);

	RenderPipelineConfigParams PipelineParams;
	PipelineParams.mVulkanRHI = mVulkanRHI;

	mRenderPipeline = std::make_shared<RenderPipeline>();
	mRenderPipeline->initialize(PipelineParams);
}
