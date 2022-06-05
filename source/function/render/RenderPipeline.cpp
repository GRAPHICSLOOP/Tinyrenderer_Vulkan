#include "RenderPipeline.h"

void tiny::RenderPipeline::initialize(const RenderPipelineConfigParams params)
{
    mVulkanRHI = params.mVulkanRHI;

	PassConfigParams passParams;
	passParams.mVulkanRHI = params.mVulkanRHI;
	passParams.mRenderResource = params.mRenderResource;

	mCameraPass = std::make_shared<MainCameraPass>();
	mCameraPass->initialize(passParams);
}

void tiny::RenderPipeline::draw(float deltaTime)
{
	mCameraPass->drawPass();
}
