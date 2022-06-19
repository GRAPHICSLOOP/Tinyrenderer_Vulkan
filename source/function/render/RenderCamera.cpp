
#include "RenderCamera.h"
#include "core/base/macro.h"

tiny::RenderCamera::RenderCamera()
{
	mForwradVector = -Z;
	mPosition = glm::vec3(0, 0, 4.f);
}

void tiny::RenderCamera::move(glm::vec3 delta)
{
	mPosition += delta.x * getRightVector();
	mPosition += delta.y * getForwardVector();
	mPosition += delta.z * getUpVector();
}

void tiny::RenderCamera::rotate(glm::vec2 delta)
{
	if (mRotation.pitch > 89.f || mRotation.pitch < -89.f)
	{
		delta.x = 0;
	}

	mRotation.pitch += delta.y;
	mRotation.yaw += delta.x;

	glm::vec3 dir;
	dir.x = glm::cos(glm::radians(mRotation.yaw)) * glm::cos(glm::radians(mRotation.pitch));
	dir.y = glm::sin(glm::radians(mRotation.pitch));
	dir.z = glm::sin(glm::radians(mRotation.yaw)) * glm::cos(glm::radians(mRotation.pitch));
	mForwradVector = glm::normalize(dir);
}

glm::mat4 tiny::RenderCamera::getViewMatrix()
{
	return glm::lookAtRH(mPosition, mPosition + getForwardVector(), getUpVector());
}

glm::vec3 tiny::RenderCamera::getUpVector()
{
	return glm::normalize(glm::cross(getForwardVector(),getRightVector()));
}

glm::vec3 tiny::RenderCamera::getRightVector()
{
	return glm::normalize(glm::cross(Y, getForwardVector()));
}

glm::vec3 tiny::RenderCamera::getForwardVector()
{
	return mForwradVector;
}
