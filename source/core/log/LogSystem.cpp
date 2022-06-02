#include "LogSystem.h"
#include <spdlog/sinks/stdout_color_sinks.h>

tiny::LogSystem::LogSystem()
{
}

tiny::LogSystem::~LogSystem()
{
	mLogger->flush();
}

void tiny::LogSystem::initialize()
{
	mLogger = spdlog::stdout_color_mt("TINY");
	mLogger->set_level(spdlog::level::trace);

	mLogger->set_pattern("[%^%l%$] %v");
}
