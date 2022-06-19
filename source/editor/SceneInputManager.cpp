#include "SceneInputManager.h"
#include "function/global/GlobalContext.h"
#include "core/base/macro.h"

void tiny::SceneInputManager::initialize()
{
	mRenderSystem = gRuntimeGlobalContext.mRenderSystem;
	mEditorMouseButtonRight = false;

	listenKeyInput();
	listenMouseInput();
}

void tiny::SceneInputManager::tick(float deltaTime)
{
	processKeyInput();
}

void tiny::SceneInputManager::onKey(int key, int scancode, int action, int mods)
{
	glm::vec2 delta = glm::vec2(0.f);
	float cameraSpeed = 0.1f;
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_W)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_FORWARD;
		if (key == GLFW_KEY_S)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_BACK;
		if (key == GLFW_KEY_D)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_RIGHT;
		if (key == GLFW_KEY_A)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_LEFT;
		if (key == GLFW_KEY_E)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_UP;
		if (key == GLFW_KEY_Q)
			mEditorCommand |= (uint32_t)EDITORCOMMNAD::CAMERA_DOWN;
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_FORWARD;
		if (key == GLFW_KEY_S)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_BACK;
		if (key == GLFW_KEY_D)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_RIGHT;
		if (key == GLFW_KEY_A)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_LEFT;
		if (key == GLFW_KEY_E)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_UP;
		if (key == GLFW_KEY_Q)
			mEditorCommand &= 0xFFFFFFFF ^ (uint32_t)EDITORCOMMNAD::CAMERA_DOWN;
	}
}

void tiny::SceneInputManager::onMouseButton(int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			mEditorMouseButtonRight = true;
			setMouseCursorMode(false);
		}
	} 
	else if (action == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			mEditorMouseButtonRight = false;
			setMouseCursorMode(true);
		}
	}
}

void tiny::SceneInputManager::onCursorPos(float xpos, float ypos)
{
	glm::vec2 currentMousePos = glm::vec2(xpos, ypos);
	float cameraRotateSpeed = 0.2f;

	if (mEditorMouseButtonRight)
	{
		glm::vec2 delta = (mLastMousePos - currentMousePos) * cameraRotateSpeed;
		mRenderSystem->mRenderCamera->rotate(delta);
	}

	mLastMousePos = currentMousePos;
}

void tiny::SceneInputManager::processKeyInput()
{
	glm::vec3 delta = glm::vec3(0.f);
	float cameraSpeed = 0.001f;
	if ((uint32_t)EDITORCOMMNAD::CAMERA_FORWARD & mEditorCommand)
	{
		delta.y += cameraSpeed;
	}
	if ((uint32_t)EDITORCOMMNAD::CAMERA_BACK & mEditorCommand)
	{
		delta.y -= cameraSpeed;
	}
	if ((uint32_t)EDITORCOMMNAD::CAMERA_RIGHT & mEditorCommand)
	{
		delta.x -= cameraSpeed;
	}
	if ((uint32_t)EDITORCOMMNAD::CAMERA_LEFT & mEditorCommand)
	{
		delta.x += cameraSpeed;
	}
	if ((uint32_t)EDITORCOMMNAD::CAMERA_UP & mEditorCommand)
	{
		delta.z += cameraSpeed;
	}
	if ((uint32_t)EDITORCOMMNAD::CAMERA_DOWN & mEditorCommand)
	{
		delta.z -= cameraSpeed;
	}

	mRenderSystem->mRenderCamera->move(delta);
}
