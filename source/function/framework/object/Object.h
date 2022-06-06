#pragma once
#include "function/framework/component/NodeComponent.h"
#include "function/inputSystem/inputSystem.h"

namespace tiny
{
	class Object : public inputSystem
	{
	public:
		Object();
		~Object();

		void onKey(int key, int scancode, int action, int mods);
	public:
		std::shared_ptr<NodeComponent> mRootComponent;
	};
}


