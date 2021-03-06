#include "WindowSystem.h"

tiny::WindowSystem::~WindowSystem()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void tiny::WindowSystem::initialize()
{
	WindowConfigParams params;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 禁止调整窗口大小
	mWindow = glfwCreateWindow(params.mWidth, params.mHeight, params.mTitle, nullptr, nullptr);


	// 设置回调
	glfwSetWindowUserPointer(mWindow,this);
	glfwSetKeyCallback(mWindow, keyCallBack);
	glfwSetMouseButtonCallback(mWindow,mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow,cursorPosCallback);
}

void tiny::WindowSystem::pollEvents() const
{
	glfwPollEvents();
}

bool tiny::WindowSystem::shouldClose() const
{
	return glfwWindowShouldClose(mWindow);
}

void tiny::WindowSystem::setMouseCursorMode(bool enable)
{
	if (enable)
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

GLFWwindow* tiny::WindowSystem::getWindow() const
{
	return mWindow;
}
