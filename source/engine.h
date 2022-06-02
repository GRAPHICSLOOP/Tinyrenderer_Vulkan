#pragma once
#include <filesystem>
#include "core/base/macro.h"

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
		void logicalTick(float delta_time);
		void rendererTick(float delta_time);
		
		void calculateFPS(float delta_time);
	protected: 
		EngineConfigParams mInitParams;
	};
}


