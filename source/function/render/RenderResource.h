#pragma once
#include <function/render/vulkan/VulkanRHI.h>
#include <glm/glm.hpp>


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

	struct Vertex
	{
	public:
		glm::vec3 mPosition;
		glm::vec3 mColor;
		glm::vec2 mTexCoord;

		static std::array<vk::VertexInputBindingDescription, 1> getBindingDescription()
		{
			std::array<vk::VertexInputBindingDescription, 1> des;
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
			des[0].offset = offsetof(Vertex, mPosition);

			des[1].binding = 0;
			des[1].location = 1;
			des[1].format = vk::Format::eR32G32B32Sfloat;
			des[1].offset = offsetof(Vertex, mColor);

			des[2].binding = 0;
			des[2].location = 2;
			des[2].format = vk::Format::eR32G32Sfloat;
			des[2].offset = offsetof(Vertex, mTexCoord);

			return des;
		}
	};

	struct TransfromUniform
	{
	public:
		glm::mat4 mModel;
		glm::mat4 mView;
		glm::mat4 mProj;
	};

	struct MeshBufferResource
	{
	public:
		uint32_t mVertexCount;
		uint32_t mIndexCount;
		vk::Buffer mVertexBuffer;
		vk::DeviceMemory mVertexBufferMemory;
		vk::Buffer mIndexBuffer;
		vk::DeviceMemory mIndexBufferMemory;
	};

	struct TransfromBufferResource
	{
	public:
		vk::Buffer mBuffer;
		vk::DeviceMemory mMemory;
	};

	struct SampleResource
	{
	public:
		vk::Sampler mTextureSampler;
		
	};

	struct Textrure2D
	{
	public:
		uint32_t mWidth;
		uint32_t mHeight;
		PIXEL_FORMAT mFormat;
		uint32_t mMiplevels;
		vk::Image mImage; 
		vk::ImageView mImageView;
		vk::DeviceMemory mMemory;
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
		void addRenderData(const void* VerticesData, uint32_t VerticesCount, const void* indicesData, uint32_t indicesCount);

	public:
		TransfromBufferResource mTransfromResource;
		SampleResource mSampleResource;
		// 先假设这个MeshBuffer有多个
		std::vector<MeshBufferResource> mMeshBufferResources;
		Textrure2D mTextureResource;

	private:
		std::shared_ptr<VulkanRHI> mVulkanRHI;

	private:
		void createTransfromUniformBuffer();
		void createTextureSampler(uint32_t mipLevels);
		void createVertexBuffer(MeshBufferResource& bufferResouce,const void* VerticesData, uint32_t count);
		void createIndexBuffer(MeshBufferResource& bufferResouce,const void* indicesData, uint32_t count);
		void tempLoadImage();
	};
}


