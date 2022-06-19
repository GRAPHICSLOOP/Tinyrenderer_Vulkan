#pragma once
#include "function/framework/component/NodeComponent.h"
#include "function/inputSystem/InputSystem.h"
#include "ObjectIDAllocator.h"

namespace tiny
{
	class Object : public inputSystem , public std::enable_shared_from_this<Object>
	{
	public:
		Object(ObjectID id);
		~Object();
		void tick(float delta);

		size_t getID();

	protected:
		void onKey(int key, int scancode, int action, int mods);

	public:
		std::shared_ptr<NodeComponent> mRootComponent;

	private:
		ObjectID mID;
	};
}


