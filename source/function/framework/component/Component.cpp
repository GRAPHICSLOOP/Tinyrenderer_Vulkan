#include "Component.h"
#include "NodeComponent.h"

tiny::Component::Component()
{
}

tiny::Component::~Component()
{
}

void tiny::Component::attachToComponent(std::weak_ptr<NodeComponent> component)
{
	mParentComponent = component;
	if(mParentComponent.lock())
		mParentComponent.lock()->addComponent(shared_from_this());
}
