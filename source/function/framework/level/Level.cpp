#include "Level.h"
#include "function/framework/component/MeshComponent.h"
#include "function/framework/component/TransformComponent.h"
#include "core/base/macro.h"

void tiny::Level::initialize()
{
	std::shared_ptr<Object> newObject = createObject();
	std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>(newObject);
	std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>(newObject);
	meshComponent->attachToComponent(newObject->mRootComponent);
	transformComponent->attachToComponent(newObject->mRootComponent);
}

void tiny::Level::tick(float delta)
{
	for (const auto obj : mObjects)
	{
		obj->tick(delta);
	}
}

std::shared_ptr<tiny::Object> tiny::Level::createObject()
{
	std::shared_ptr<Object> newObject = std::make_shared<Object>(ObjectIDAllocator::Alloc());
	addObject(newObject);
	return newObject;
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
