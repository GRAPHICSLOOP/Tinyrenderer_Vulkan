#pragma once
#include <memory>
#include "function/render/Pass/CameraPass.h"

namespace tiny
{
	struct RenderPipelineConfigParams
	{
		std::shared_ptr<VulkanRHI> mVulkanRHI;
	};

	class RenderPipeline
	{
	public:
		void initialize(const RenderPipelineConfigParams params);

	private:
		std::shared_ptr<MainCameraPass> mCameraPass;
	};
}


