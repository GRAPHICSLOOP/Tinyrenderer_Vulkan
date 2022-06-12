#pragma once
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/renderResource/RenderResource.h"
#include "function/render/renderResource/TextureResource.h"


namespace tiny
{
	class VulkanUtil
	{
	public:
        static uint32_t findMemoryType(
            uint32_t typeFilter,
            vk::MemoryPropertyFlags property);

        static void createImage(
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
            vk::ImageAspectFlags aspectMask,
            vk::Format fomat,
            vk::Image image);

        static std::vector<char> readFile(const char* fileName);

        static vk::ShaderModule createShaderModule(const std::vector<char>& code);

        static vk::ShaderModule loadShaderModuleFromFile(const char* fileName);

        static void createBuffer(
            vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            vk::Buffer& buffer,
            vk::DeviceMemory& bufferMemory);

        static void transitionImageLayout(
            vk::Image& image,
            vk::Format format,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            uint32_t mipLevel);

        static void copyBuffer(
            vk::Buffer srcBuffer,
            vk::Buffer dstBuffer,
            vk::DeviceSize size);

        static tiny::TextureBufferResource createTextureBufferResource(
            uint32_t width,
            uint32_t height,
            void* pixels,
            PIXEL_FORMAT pixelFormat,
            uint32_t miplevels);

        static void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

        static void generateMipmaps(vk::Image image, uint32_t width, uint32_t hegith, uint32_t miplevels);
	};
}


