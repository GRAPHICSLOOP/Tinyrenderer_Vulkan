#include "Object.h"
#include "core/base/macro.h"
#include "function/framework/component/MeshComponent.h"
#include "function/framework/component/TransformComponent.h"

tiny::Object::Object(ObjectID id)
{
	mID = id;
	mRootComponent = std::make_shared<NodeComponent>();

	listenKeyInput();
}

tiny::Object::~Object()
{
	
}

void tiny::Object::tick(float delta)
{
	auto comps = mRootComponent->getComponents();
	for (auto& com : comps)
	{
		com->tick(delta);
	}

}

size_t tiny::Object::getID()
{
	return mID;
}

void tiny::Object::onKey(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A)
	{
		static bool created = false;
		if (!created)
		{
			MeshComponent* meshComponent = mRootComponent->getComponent<MeshComponent>();
			if (meshComponent)
			{
				//meshComponent->TempLoadMeshAsset("TestNewMesh", "models/viking_room.obj");
				meshComponent->TempLoadMeshAsset("TestNewMesh", "models/nanosuit/nanosuit.obj");
			}
			else
			{
				TINYLOG_ERROR("meshComponent NULL");
			}

			TransformComponent* transformComponent = mRootComponent->getComponent<TransformComponent>();
			if (transformComponent)
			{
				transformComponent->setScale(glm::vec3(0.1f));
			}

			created = true;
		}
	}
}
