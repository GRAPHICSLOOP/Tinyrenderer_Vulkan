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
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		windowSystem->pollEvents();
		logicalTick(deltaTime);
		rendererTick(deltaTime);
		clear();
	}

	gRuntimeGlobalContext.mRenderSystem->mVulkanRHI->mDevice.waitIdle();
}


void tiny::TinyEngine::initialize()
{
	mLevel = std::make_shared<Level>();
	mLevel->initialize();
	mScenceManager = std::make_shared<SceneInputManager>();
	mScenceManager->initialize();
}

void tiny::TinyEngine::clear()
{
	// 清理相关临时数据

}

void tiny::TinyEngine::logicalTick(float deltaTime)
{
	mLevel->tick(deltaTime);
	mScenceManager->tick(deltaTime);
}

void tiny::TinyEngine::rendererTick(float deltaTime)
{
	gRuntimeGlobalContext.mRenderSystem->tick(deltaTime);
}

void tiny::TinyEngine::calculateFPS(float deltaTime)
{
}
