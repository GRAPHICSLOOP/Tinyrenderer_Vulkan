#pragma once
#include <memory>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/RenderPipeline.h"
#include "function/render/renderResource/RenderResource.h"
#include "RenderCamera.h"

namespace tiny
{
	class RenderSystem
	{
	public:
		void initialize();
		void tick(float deltaTime);

	public:
		std::shared_ptr<tiny::VulkanRHI> mVulkanRHI;
		std::shared_ptr<tiny::RenderResource> mRenderResource;
		std::shared_ptr<tiny::RenderCamera> mRenderCamera;

	private:
		std::shared_ptr<tiny::RenderPipeline> mRenderPipeline;
	};
}


