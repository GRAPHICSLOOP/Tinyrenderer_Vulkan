#pragma once
#include <stdint.h>
#include <unordered_map>
#include "function/render/vulkan/VulkanRHI.h"

namespace tiny
{
	enum class EAttachmentType : uint8_t
	{
		color,
		depth
	};

	struct FrameBufferAttachment
	{
	public:
		vk::Image mImage;
		vk::ImageView mImageView;
		vk::DeviceMemory mMemory;
		vk::Format mFormat;
	};

	struct Framebuffer
	{
	public:
		vk::RenderPass mRenderPass;
		std::unordered_map<EAttachmentType,FrameBufferAttachment> mAttachments;
	public:
		std::vector<FrameBufferAttachment> getAttachments()
		{
			std::vector<FrameBufferAttachment> result;
			for (const auto& attachment : mAttachments)
			{
				result.push_back(attachment.second);
			}

			return result;
		}
	};

	struct PassConfigParams
	{
	public:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
	};

	class MainCameraPass
	{
	public:
		~MainCameraPass();
		void initialize(PassConfigParams params);

	private:
		Framebuffer mFrameBuffer;
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		vk::RenderPass mRenderPass;


	private:
		void setupAttachments();
		void setupRenderPass();
		void setupDescriptorSetLayout();
	};
}


