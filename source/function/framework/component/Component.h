#pragma once
#include <memory>

namespace tiny
{
	class Component : public std::enable_shared_from_this<Component>
	{
	public:
		Component();
		Component(std::shared_ptr<class Object> obj);
		virtual ~Component();

	public:
		void attachToComponent(std::weak_ptr<class NodeComponent> component);
		virtual void tick(float delta);
		
	protected:
		std::weak_ptr<class NodeComponent> mParentComponent;
		std::weak_ptr<class Object> mObject;
	};
}


