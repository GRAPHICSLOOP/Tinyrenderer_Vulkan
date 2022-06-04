#include "RenderPipeline.h"

void tiny::RenderPipeline::initialize(const RenderPipelineConfigParams params)
{
    mVulkanRHI = params.mVulkanRHI;

	PassConfigParams passConfigParams;
	passConfigParams.mVulkanRHI = params.mVulkanRHI;
	passConfigParams.mRenderResource = params.mRenderResource;

	mCameraPass = std::make_shared<MainCameraPass>();
	mCameraPass->initialize(passConfigParams);
}

void tiny::RenderPipeline::draw(float deltaTime)
{
	mCameraPass->drawPass();
}
