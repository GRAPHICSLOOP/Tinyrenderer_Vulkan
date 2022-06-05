#pragma once
#include "function/framework/component/NodeComponent.h"

namespace tiny
{
	class Object
	{
	public:
		Object();
		~Object();

	public:
		std::shared_ptr<NodeComponent> mRootComponent;
	};
}


