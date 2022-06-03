#pragma once
#include <memory>
#include "function/render/vulkan/VulkanRHI.h"

namespace tiny
{
	class RenderSystem
	{
	public:
		void initialize();

	public:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
	};
}


