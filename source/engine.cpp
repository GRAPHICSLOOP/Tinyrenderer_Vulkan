#include "Engine.h"
#include "function/global/GlobalContext.h"

void tiny::TinyEngine::startEngine(const EngineConfigParams& params)
{
	mInitParams = params;
	gRuntimeGlobalContext.startSystems();
	
	LOG_INFO("Engine start!");
}

void tiny::TinyEngine::shutdownEngine()
{
}

void tiny::TinyEngine::run()
{
}


void tiny::TinyEngine::initialize()
{
}

void tiny::TinyEngine::clear()
{
}

void tiny::TinyEngine::logicalTick(float delta_time)
{
}

void tiny::TinyEngine::rendererTick(float delta_time)
{
}

void tiny::TinyEngine::calculateFPS(float delta_time)
{
}
