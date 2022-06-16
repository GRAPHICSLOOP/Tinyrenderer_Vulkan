#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace tiny
{
	enum DESCRIPTOR_TYPE
	{
		DESCRIPTOR_TYPE_UNIFORM = 0,
		DESCRIPTOR_TYPE_SAMPLE = 1,
		DESCRIPTOR_TYPE_COUNT = 2
	};

	struct VertexBufferData
	{
	public:
		glm::vec3 mPosition;
		glm::vec3 mColor;
		glm::vec2 mTexCoord;

		static std::array<vk::VertexInputBindingDescription, 1> getBindingDescription()
		{
			std::array<vk::VertexInputBindingDescription, 1> des;
			des[0].binding = 0;
			des[0].stride = sizeof(VertexBufferData);
			des[0].inputRate = vk::VertexInputRate::eVertex;

			return des;
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<vk::VertexInputAttributeDescription, 3> des;
			des[0].binding = 0;
			des[0].location = 0;
			des[0].format = vk::Format::eR32G32B32Sfloat;
			des[0].offset = offsetof(VertexBufferData, mPosition);

			des[1].binding = 0;
			des[1].location = 1;
			des[1].format = vk::Format::eR32G32B32Sfloat;
			des[1].offset = offsetof(VertexBufferData, mColor);

			des[2].binding = 0;
			des[2].location = 2;
			des[2].format = vk::Format::eR32G32Sfloat;
			des[2].offset = offsetof(VertexBufferData, mTexCoord);

			return des;
		}
	};

	struct CameraBufferData
	{
	public:
		glm::mat4 mView;
		glm::mat4 mProj;
		glm::mat4 mViewPorj;
	};

	struct ObjectBufferData
	{
	public:
		glm::mat4 mModel;
	};

	struct CameraBufferResource
	{
	public:
		CameraBufferResource()
		{
			mData = &privateData;
		}

	public:
		void* mData;
		vk::Buffer mBuffer;
		vk::DeviceMemory mMemory;
	private:
		CameraBufferData privateData;
	};

	struct ObjectBufferResource
	{
	public:
		ObjectBufferResource()
		{
			mData = &privateData;
		}

	public:
		void* mData;
		vk::Buffer mBuffer;
		vk::DeviceMemory mMemory;
	private:
		ObjectBufferData privateData;
	};
}
