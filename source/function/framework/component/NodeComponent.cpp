#include "NodeComponent.h"

tiny::NodeComponent::NodeComponent()
{
}

tiny::NodeComponent::NodeComponent(std::shared_ptr<class Object> obj)
	:Component(obj)
{
}

void tiny::NodeComponent::addComponent(std::shared_ptr<Component> comp)
{
	mComponents.push_back(comp);
}

std::vector<std::shared_ptr<tiny::Component>> tiny::NodeComponent::getComponents()
{
	return mComponents;
}

