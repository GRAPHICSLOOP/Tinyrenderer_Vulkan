#include "NodeComponent.h"

void tiny::NodeComponent::addComponent(std::shared_ptr<Component> comp)
{
	mComponents.push_back(comp);
}

