#pragma once
#include <glm/glm.hpp>
#include "Component.h"
#include "core/math/MathCommon.h"

namespace tiny
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent(std::shared_ptr<class Object> obj);
		
		void setPosistion(glm::vec3 pos);
		void setRotation(tiny::Rotation rot);
		void setScale(glm::vec3 scale);
		glm::vec3 getPosistion();
		tiny::Rotation getRotation();
		glm::vec3 getScale();

		glm::mat4 getModelMatrix();

	public:
		

	private:
		glm::vec3 mPosition;
		Rotation mRotation;
		glm::vec3 mScale;
		glm::mat4 mModelMatrix;
		glm::mat4 mTranslateMatrix;
		glm::mat4 mScaleMatrix;
		glm::mat4 mRotationMatrix;
		bool dirty;
	};
}


