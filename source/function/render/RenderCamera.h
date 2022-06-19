#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/math/MathCommon.h"

namespace tiny
{


	class RenderCamera
	{
	public:
		RenderCamera();

		static const glm::vec3 X, Y, Z;

		float mFovx;
		float mFovy;
		float mAspect;
		float mNear;
		float mFar;

	public:
		void move(glm::vec3 delta);
		void rotate(glm::vec2 delta);
		glm::mat4 getViewMatrix();

		glm::vec3 getUpVector();
		glm::vec3 getRightVector();
		glm::vec3 getForwardVector();
	private:
		glm::vec3 mPosition;
		glm::vec3 mForwradVector;
		Rotation mRotation;
	};

	inline const glm::vec3 RenderCamera::X = glm::vec3(1.f, 0.f, 0.f);
	inline const glm::vec3 RenderCamera::Y = glm::vec3(0.f, 1.f, 0.f);
	inline const glm::vec3 RenderCamera::Z = glm::vec3(0.f, 0.f, 1.f);
}

