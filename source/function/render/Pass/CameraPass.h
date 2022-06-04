#pragma once
#include <stdint.h>
#include <unordered_map>
#include "function/render/vulkan/VulkanRHI.h"
#include "function/render/RenderResource.h"

namespace tiny
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoord;

		static std::array<vk::VertexInputBindingDescription, 1> getBindingDescription()
		{
			std::array<vk::VertexInputBindingDescription,1> des;
			des[0].binding = 0;
			des[0].stride = sizeof(Vertex);
			des[0].inputRate = vk::VertexInputRate::eVertex;

			return des;
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<vk::VertexInputAttributeDescription, 3> des;
			des[0].binding = 0;
			des[0].location = 0;
			des[0].format = vk::Format::eR32G32B32Sfloat;
			des[0].offset = offsetof(Vertex, position);

			des[1].binding = 0;
			des[1].location = 1;
			des[1].format = vk::Format::eR32G32B32Sfloat;
			des[1].offset = offsetof(Vertex, color);

			des[2].binding = 0;
			des[2].location = 2;
			des[2].format = vk::Format::eR32G32Sfloat;
			des[2].offset = offsetof(Vertex, texCoord);

			return des;
		}
	};

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
		std::shared_ptr<RenderResource> mRenderResource;
	};

	class MainCameraPass
	{
	public:
		~MainCameraPass();
		void initialize(PassConfigParams params);

	private:
		Framebuffer mFrameBuffer;
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::shared_ptr<RenderResource> mRenderResource;
		vk::RenderPass mRenderPass;
		vk::DescriptorSetLayout mDescSetLayout;
		vk::PipelineLayout mPipelineLayout;
		vk::Pipeline mPipeline;
		std::vector<vk::DescriptorSet> mDescriptorSets;
	private:
		void setupAttachments();
		void setupRenderPass();
		void setupDescriptorSetLayout();
		void setupPipelines();
		void setupDescriptorSet();
	};
}


