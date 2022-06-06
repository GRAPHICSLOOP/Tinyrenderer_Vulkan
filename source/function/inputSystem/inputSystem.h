#pragma once
namespace tiny
{
	class inputSystem
	{
	public:
		void listenKeyInput();

		// 子类可重写
		virtual void onKey(int key, int scancode, int action, int mods)
		{

		}
	};
}

