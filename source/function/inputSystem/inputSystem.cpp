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

void tiny::inputSystem::listenMouseInput()
{
	std::shared_ptr<WindowSystem> windowSystem = gRuntimeGlobalContext.mWindowSystem;
	windowSystem->registerOnMouseButtonFunc(std::bind(&inputSystem::onMouseButton,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));

	windowSystem->registerOnCursorPosFunc(std::bind(&inputSystem::onCursorPos,
		this,
		std::placeholders::_1,
		std::placeholders::_2));
}

void tiny::inputSystem::setMouseCursorMode(bool enable)
{
	std::shared_ptr<WindowSystem> windowSystem = gRuntimeGlobalContext.mWindowSystem;
	windowSystem->setMouseCursorMode(enable);
}
