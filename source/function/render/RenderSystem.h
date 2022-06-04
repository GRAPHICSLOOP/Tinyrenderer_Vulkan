#pragma once
#include <memory>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/RenderPipeline.h"
#include "function/render/RenderResource.h"

namespace tiny
{

	class RenderSystem
	{
	public:
		void initialize();
		void tick(float deltaTime);
	public:
		std::shared_ptr<VulkanRHI> mVulkanRHI;

	private:
		std::shared_ptr<RenderResource> mRenderResource;
		std::shared_ptr<RenderPipeline> mRenderPipeline;

	};
}


