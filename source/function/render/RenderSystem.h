#pragma once
#include <memory>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/RenderPipeline.h"

namespace tiny
{
	class RenderSystem
	{
	public:
		void initialize();

	private:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<RenderPipeline> mRenderPipeline;
	};
}


