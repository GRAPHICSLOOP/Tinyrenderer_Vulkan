#pragma once
#include <vector>
#include "Component.h"

namespace tiny
{
	class NodeComponent : public Component
	{
	public:
		NodeComponent();
		NodeComponent(std::shared_ptr<class Object> obj);

		void addComponent(std::shared_ptr<Component> comp);
		std::vector<std::shared_ptr<Component>> getComponents();

		template<typename _T>
		std::shared_ptr<_T> getComponent_SharedPtr();
		
		template<typename _T>
		_T* getComponent();

	private:
		std::vector<std::shared_ptr<Component>> mComponents;
	};


	template<typename _T>
	inline std::shared_ptr<_T> NodeComponent::getComponent_SharedPtr()
	{
		std::shared_ptr<_T> result;
		for (std::shared_ptr<tiny::Component> comp : mComponents)
		{
			// dynamic_pointer_cast 基类需要有虚函数
			result = std::dynamic_pointer_cast<_T>(comp);
			if (result)
			{
				return result;
			}

		}

		return nullptr;
	}

	template<typename _T>
	inline _T* NodeComponent::getComponent()
	{
		_T* result;
		for (std::shared_ptr<tiny::Component> comp : mComponents)
		{
			result = dynamic_cast<_T*>(comp.get());
			if (result)
			{
				return result;
			}

		}

		return nullptr;
	}
}


