#include "Object.h"

tiny::Object::Object()
{
	mRootComponent = std::make_shared<NodeComponent>();
}

tiny::Object::~Object()
{
}
