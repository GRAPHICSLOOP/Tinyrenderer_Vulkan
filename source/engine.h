#pragma once
#include <filesystem>
#include "core/base/macro.h"
#include "function/framework/level/Level.h"

namespace tiny 
{
	struct EngineConfigParams
	{
		std::filesystem::path mConfigFilePath;
	};

	class TinyEngine
	{
	public:
		void startEngine(const EngineConfigParams& params);
		void shutdownEngine();
		void run();

		void initialize();
		void clear();
	protected:
		void logicalTick(float deltaTime);
		void rendererTick(float deltaTime);
		
		void calculateFPS(float deltaTime);
	protected: 
		EngineConfigParams mInitParams;

	private:
		std::shared_ptr<Level> mLevel;
	};
}


