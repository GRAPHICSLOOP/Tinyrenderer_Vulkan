#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace tiny
{
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
}
