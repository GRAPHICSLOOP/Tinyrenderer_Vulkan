#pragma once
#include <glm/glm.hpp>
#include "Component.h"

namespace tiny
{
	class TransformComponent : public Component
	{
	public:
		glm::vec3 mPosition;
		glm::vec3 mRotation;
		glm::vec3 mScale;
	};
}


