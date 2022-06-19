#pragma once
namespace tiny
{
	class inputSystem
	{
	public:
		void listenKeyInput();
		void listenMouseInput();
		void setMouseCursorMode(bool enable);

		// 子类可重写
		virtual void onKey(int key, int scancode, int action, int mods)
		{

		}

		// 子类可重写
		virtual void onMouseButton(int button, int action, int mods)
		{

		}

		// 子类可重写
		virtual void onCursorPos(float xpos, float ypos)
		{

		}

	};
}

