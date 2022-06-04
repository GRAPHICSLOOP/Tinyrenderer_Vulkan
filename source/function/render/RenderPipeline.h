#pragma once
#include <memory>
#include "function/render/Pass/CameraPass.h"
#include "function/render/RenderResource.h"

namespace tiny
{
	struct RenderPipelineConfigParams
	{
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<RenderResource> mRenderResource;
	};

	class RenderPipeline
	{
	public:
		void initialize(const RenderPipelineConfigParams params);

	private:
		std::shared_ptr<MainCameraPass> mCameraPass;
	};
}


