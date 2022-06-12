#include "InputSystem.h"
#include "function/global/GlobalContext.h"
#include "core/base/macro.h"

void tiny::inputSystem::listenKeyInput()
{
	std::shared_ptr<WindowSystem> windowSystem = gRuntimeGlobalContext.mWindowSystem;
	windowSystem->registerOnKeyFunc(std::bind(&inputSystem::onKey,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4));

}
