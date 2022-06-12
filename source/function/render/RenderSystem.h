#pragma once
#include <memory>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/RenderPipeline.h"
#include "function/render/renderResource/RenderResource.h"

namespace tiny
{

	class RenderSystem
	{
	public:
		void initialize();
		void tick(float deltaTime);

	public:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<RenderResource> mRenderResource;

	private:
		std::shared_ptr<RenderPipeline> mRenderPipeline;
	};
}


