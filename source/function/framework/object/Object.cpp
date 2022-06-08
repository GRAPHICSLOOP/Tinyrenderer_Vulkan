#include "Object.h"
#include "core/base/macro.h"
#include "function/framework/component/MeshComponent.h"

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
		static bool created = false;
		if (!created)
		{
			std::shared_ptr<MeshComponent> meshComponent = mRootComponent->getComponent<MeshComponent>();
			if (meshComponent)
			{
				meshComponent->TempLoadMeshAsset("TestNewMesh", "models/viking_room.obj");
			}
			else
			{
				TINYLOG_ERROR("meshComponent NULL");
			}

			created = true;
		}
	}
}
