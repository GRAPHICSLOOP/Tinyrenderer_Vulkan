#pragma once
#include "function/render/vulkan/VulkanRHI.h"

namespace tiny
{
	enum class PIXEL_FORMAT : uint8_t
	{
		PIXEL_FORMAT_UNKNOWN = 0,
		PIXEL_FORMAT_R8G8B8_UNORM,
		PIXEL_FORMAT_R8G8B8_SRGB,
		PIXEL_FORMAT_R8G8B8A8_UNORM,
		PIXEL_FORMAT_R8G8B8A8_SRGB,
		PIXEL_FORMAT_R32G32_FLOAT,
		PIXEL_FORMAT_R32G32B32_FLOAT,
		PIXEL_FORMAT_R32G32B32A32_FLOAT
	};

	struct TextureBufferResource
	{
	public:
		vk::Image mImage;
		vk::ImageView mImageView;
		vk::DeviceMemory mMemory;
		vk::Sampler mTextureSampler;
		vk::DescriptorSet mDescriptorSet;
	};

	class TextureResource : public std::enable_shared_from_this<TextureResource>
	{
	public:
		TextureResource();
		void initialize(
			size_t id,
			uint32_t width,
			uint32_t height,
			void* pixels,
			PIXEL_FORMAT pixelFormat,
			uint32_t miplevels);

		~TextureResource();

	public:
		TextureBufferResource mTextureBufferResource;
	
	private:
		vk::Sampler createTextureSampler(uint32_t mipLevels);
		void createDescriptorSet();

	private:
		size_t mId;
		bool mInit;
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<class RenderResource> mRenderResource;
	};
}


