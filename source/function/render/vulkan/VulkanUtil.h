#pragma once
#include <vulkan/vulkan.hpp>

namespace tiny
{
	class VulkanUtil
	{
	public:
		static vk::ImageView createImageView(vk::Device device, vk::ImageAspectFlags aspectMask, vk::Format fomat, vk::Image image);
	};
}


