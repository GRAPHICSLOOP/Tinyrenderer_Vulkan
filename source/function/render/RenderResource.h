#pragma once
#include <function/render/vulkan/VulkanRHI.h>
#include <glm/glm.hpp>


namespace tiny
{
	struct MeshBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct MeshResource
	{
	public:
		vk::Buffer mBuffer;
		vk::DeviceMemory mMemory;
	};

	struct SampleResource
	{
	public:
		vk::Sampler mTextureSampler;
		vk::Image mImage; // 临时
		vk::ImageView mImageView; // 临时
		vk::DeviceMemory mMemory; // 临时
	};

	struct RenderResourceConfigParams
	{
		std::shared_ptr<VulkanRHI> mVulkanRHI;
	};

	class RenderResource
	{
	public:
		~RenderResource();
		void initialize(const RenderResourceConfigParams& params);

	public:
		MeshResource mMeshResource;
		SampleResource mSampleResource;

	private:
		std::shared_ptr<VulkanRHI> mVulkanRHI;

	private:
		void createMeshUniformBuffer();
		void createTextureSampler(uint32_t mipLevels);
	};
}


