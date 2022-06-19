#pragma once
#include <windows.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>

namespace tiny
{
	struct WindowConfigParams
	{
		uint32_t mWidth{ 1280 };
		uint32_t mHeight{ 720 };
		const char* mTitle{ "Tinyrenderer" };
	};

	typedef std::function<void(int, int, int, int)> onKeyFunc;
	typedef std::function<void(int, int, int)> onMouseButtonFunc;
	typedef std::function<void(float, float)> onCursorPosFunc;

	class WindowSystem
	{
	public:
		~WindowSystem();
		void initialize();
		void pollEvents() const;
		bool shouldClose() const;
		GLFWwindow* getWindow() const;

		void registerOnKeyFunc(onKeyFunc func) { mOnKeyFunc.push_back(func); }
		void registerOnMouseButtonFunc(onMouseButtonFunc func) { mOnMouseButtonFunc.push_back(func); }
		void registerOnCursorPosFunc(onCursorPosFunc func) { mOnCursorPosFunc.push_back(func); }

		static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
			for (const auto& fun : app->mOnKeyFunc)
				fun(key, scancode, action, mods);
		}
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
			for (const auto& fun : app->mOnMouseButtonFunc)
				fun(button, action, mods);
		}
		static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
			for (const auto& fun : app->mOnCursorPosFunc)
				fun((float)xpos, (float)ypos);
		}

	private:
		std::vector<onKeyFunc> mOnKeyFunc;
		std::vector<onMouseButtonFunc> mOnMouseButtonFunc;
		std::vector<onCursorPosFunc> mOnCursorPosFunc;

	private:
		GLFWwindow* mWindow;
	};

}


