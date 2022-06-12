#pragma once
#include <stdint.h>
#include <unordered_map>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/renderResource/RenderResource.h"

namespace tiny
{

	enum class ATTACHMENT_TYPE : uint8_t
	{
		TYPE_DEPTH
	};

	struct FrameBufferAttachment
	{
	public:
		vk::Image mImage;
		vk::ImageView mImageView;
		vk::DeviceMemory mMemory;
		vk::Format mFormat;
	};

	struct Frame
	{
	public:
		std::vector<vk::Framebuffer> mFramebuffer;
		vk::RenderPass mRenderPass;
		std::unordered_map<ATTACHMENT_TYPE,FrameBufferAttachment> mAttachments;
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
		std::shared_ptr<RenderResource> mRenderResource;
	};

	class MainCameraPass
	{
	public:
		~MainCameraPass();
		void initialize(PassConfigParams params);
		void drawPass();

	private:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<RenderResource> mRenderResource;
		std::vector<vk::DescriptorSetLayout> mDescSetLayouts;
		vk::PipelineLayout mPipelineLayout;
		vk::Pipeline mPipeline;
		std::vector<vk::DescriptorSet> mDescriptorSets;
		Frame mFrame;

	private:
		void setupAttachments();
		void setupRenderPass();
		void setupDescriptorSetLayout();
		void setupPipelines();
		void setupDescriptorSet();
		void setupSwapchainFramebuffers();
		void TempUpdateUniformBuffer(float deltaTime);
	};
}


