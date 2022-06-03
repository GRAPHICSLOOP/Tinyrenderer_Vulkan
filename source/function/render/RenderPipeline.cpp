#include "RenderPipeline.h"

void tiny::RenderPipeline::initialize(const RenderPipelineConfigParams params)
{
	PassConfigParams passConfigParams;
	passConfigParams.mVulkanRHI = params.mVulkanRHI;

	mCameraPass = std::make_shared<MainCameraPass>();
	mCameraPass->initialize(passConfigParams);
}
