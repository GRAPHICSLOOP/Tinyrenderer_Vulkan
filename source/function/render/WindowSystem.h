#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace tiny
{
	struct WindowConfigParams
	{
		uint32_t mWidth{ 1280 };
		uint32_t mHeight{ 720 };
		const char* mTitle{ "Tinyrenderer" };
	};

	class WindowSystem
	{
	public:
		~WindowSystem();
		void initialize();
		void pollEvents() const;
		bool shouldClose() const;
		GLFWwindow* getWindow() const;

	private:
		GLFWwindow* mWindow;
	};

}


