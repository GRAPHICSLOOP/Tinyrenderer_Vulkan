#include "Engine.h"
#include "function/global/GlobalContext.h"
#include "function/render/WindowSystem.h"

void tiny::TinyEngine::startEngine(const EngineConfigParams& params)
{
	mInitParams = params;
	gRuntimeGlobalContext.startSystems();

	TINYLOG_INFO("Start Engine!!");
}

void tiny::TinyEngine::shutdownEngine()
{
}

void tiny::TinyEngine::run()
{
	std::shared_ptr<WindowSystem> windowSystem = gRuntimeGlobalContext.mWindowSystem;
	while (!windowSystem->shouldClose())
	{
		windowSystem->pollEvents();
		logicalTick(0.01f);
		rendererTick(0.01f);
	}

	gRuntimeGlobalContext.mRenderSystem->mVulkanRHI->mDevice.waitIdle();
}


void tiny::TinyEngine::initialize()
{
}

void tiny::TinyEngine::clear()
{
}

void tiny::TinyEngine::logicalTick(float deltaTime)
{
}

void tiny::TinyEngine::rendererTick(float deltaTime)
{
	gRuntimeGlobalContext.mRenderSystem->tick(deltaTime);
}

void tiny::TinyEngine::calculateFPS(float deltaTime)
{
}
