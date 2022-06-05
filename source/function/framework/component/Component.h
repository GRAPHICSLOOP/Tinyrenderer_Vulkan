#pragma once
#include <memory>

namespace tiny
{
	class Component : public std::enable_shared_from_this<Component>
	{
	public:
		Component();
		virtual ~Component();

	public:
		void attachToComponent(std::weak_ptr<class NodeComponent> component);
		
	protected:
		std::weak_ptr<class NodeComponent> mParentComponent;
	};
}


