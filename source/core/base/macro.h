#pragma once
#include "core/log/LogSystem.h"
#include "function/global/GlobalContext.h"

#define LOG_HELPER(LOG_LEVEL,...) \
	tiny::gRuntimeGlobalContext.mLogSystem->log(LOG_LEVEL,"[" + std::string(__FUNCTION__) + "]" + __VA_ARGS__);


#define LOG_DEBUG(...) LOG_HELPER(tiny::ELogLevel::debug,__VA_ARGS__);
#define LOG_ERROR(...) LOG_HELPER(tiny::ELogLevel::error,__VA_ARGS__);
#define LOG_INFO(...) LOG_HELPER(tiny::ELogLevel::info,__VA_ARGS__);
#define LOG_WARN(...) LOG_HELPER(tiny::ELogLevel::warn,__VA_ARGS__);

