#include "Level.h"
#include "function/framework/component/MeshComponent.h"
#include "function/framework/component/TransformComponent.h"
#include "core/base/macro.h"

void tiny::Level::initialize()
{
	std::shared_ptr<Object> newObject = std::make_shared<Object>();
	std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
	std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>();
	meshComponent->attachToComponent(newObject->mRootComponent);
	transformComponent->attachToComponent(newObject->mRootComponent);
	//meshComponent->TempLoadMeshAsset("TestNewMesh", "models/viking_room.obj");

	addObject(newObject);
}

void tiny::Level::tick(float deltaTime)
{
	static bool created = false;
	if (!created)
	{
		std::shared_ptr<MeshComponent> meshComponent = mObjects[0]->mRootComponent->getComponent<MeshComponent>();
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

void tiny::Level::addObject(std::shared_ptr<Object> object)
{
	mObjects.push_back(object);
}

void tiny::Level::removeObject(std::shared_ptr<Object> object)
{
	mObjects.emplace_back(object);
	mObjects.pop_back();
}

std::vector<std::shared_ptr<tiny::Object>> tiny::Level::getObjects()
{
	return mObjects;
}
