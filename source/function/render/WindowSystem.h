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

	class WindowSystem
	{
	public:
		~WindowSystem();
		void initialize();
		void pollEvents() const;
		bool shouldClose() const;
		GLFWwindow* getWindow() const;

	public:
		void registerOnKeyFunc(onKeyFunc func) { mOnKeyFunc.push_back(func); }

	public:
		static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
			for (const auto& fun : app->mOnKeyFunc)
				fun(key, scancode, action, mods);
		}

	private:
		std::vector<onKeyFunc> mOnKeyFunc;

	private:
		GLFWwindow* mWindow;
	};

}


