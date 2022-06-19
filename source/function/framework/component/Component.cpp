#include "Component.h"
#include "NodeComponent.h"
#include "function/framework/object/Object.h"

tiny::Component::Component()
{
}

tiny::Component::Component(std::shared_ptr<Object> obj)
{
	mObject = obj;
}

tiny::Component::~Component()
{
}

void tiny::Component::attachToComponent(std::weak_ptr<NodeComponent> component)
{
	mParentComponent = component;
	if (mParentComponent.lock())
	{
		mParentComponent.lock()->addComponent(shared_from_this());
	}
}

void tiny::Component::tick(float delta)
{

}
