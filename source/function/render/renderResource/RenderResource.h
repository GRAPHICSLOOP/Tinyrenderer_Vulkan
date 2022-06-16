#pragma once
#include <glm/glm.hpp>
#include "function/render/vulkan/VulkanRHI.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "RenderData.h"


namespace tiny
{
	struct ModelRenderResource
	{
	public:
		std::weak_ptr<class MeshResource> mMeshResource;
		std::weak_ptr<class TextureResource> mTextureResource;
	};

	struct RenderResourceConfigParams
	{
	public:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
	};

	class RenderResource
	{
	public:
		~RenderResource();
		void initialize(const RenderResourceConfigParams& params);
		void createVertexBuffer(struct MeshBufferResource& bufferResouce, const void* VerticesData, uint32_t count);
		void createIndexBuffer(struct MeshBufferResource& bufferResouce, const void* indicesData, uint32_t count);
		vk::DescriptorSetLayout getDescriptorSetLayout(DESCRIPTOR_TYPE type);

	public:
		ObjectBufferResource mObjectBufferResource;
		CameraBufferResource mCameraBufferResource;
		std::vector<ModelRenderResource> mModelRenderResource;
		std::unordered_map<size_t,std::weak_ptr<TextureResource>> mGlobalTextureResources;

	private:
		void createBufferResource();
		void createDescriptorSetLayout();

	private:
		std::shared_ptr<VulkanRHI> mVulkanRHI;
		std::array<vk::DescriptorSetLayout,2> mDescSetLayouts;
	};
}


