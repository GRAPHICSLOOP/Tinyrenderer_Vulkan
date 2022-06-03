#pragma once
#include <vulkan/vulkan.hpp>

namespace tiny
{
	class VulkanUtil
	{
	public:
        static uint32_t findMemoryType(
            vk::PhysicalDevice phyDevice,
            uint32_t typeFilter,
            vk::MemoryPropertyFlags property);

        static void createImage(
            vk::PhysicalDevice phyDevice,
            vk::Device device,
            uint32_t imageWidth,
            uint32_t imageHeight,
            vk::Format format,
            vk::ImageTiling tilling,
            vk::ImageUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            vk::Image& image,
            vk::DeviceMemory& imageMemory,
            uint32_t mipLevel = 1,
            vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1);

		static vk::ImageView createImageView(
            vk::Device device,
            vk::ImageAspectFlags aspectMask,
            vk::Format fomat,
            vk::Image image);


	};
}


