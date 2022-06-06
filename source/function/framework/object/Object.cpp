#include "Object.h"
#include "core/base/macro.h"

tiny::Object::Object()
{
	mRootComponent = std::make_shared<NodeComponent>();

	listenKeyInput();
}

tiny::Object::~Object()
{
}

void tiny::Object::onKey(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A)
	{
		TINYLOG_DEBUG("Object AAAA");
	}
}
