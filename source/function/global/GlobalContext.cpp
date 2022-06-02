#include "GlobalContext.h"

tiny::RuntimeGlobalContext tiny::gRuntimeGlobalContext;

void tiny::RuntimeGlobalContext::startSystems()
{
	mLogSystem = std::make_shared<LogSystem>();
	mLogSystem->initialize();

	mWindowSystem = std::make_shared<WindowSystem>();
	mWindowSystem->initialize();
	
	mRenderSystem = std::make_shared<RenderSystem>();
}

void tiny::RuntimeGlobalContext::shutdownSystems()
{
	mLogSystem.reset();
	mWindowSystem.reset();
	mRenderSystem.reset();
}
