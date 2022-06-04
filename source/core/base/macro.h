#pragma once
#include "core/log/LogSystem.h"
#include "function/global/GlobalContext.h"

#define LOG_HELPER(LOG_LEVEL,...) \
	tiny::gRuntimeGlobalContext.mLogSystem->log(LOG_LEVEL,"[" + std::string(__FUNCTION__) + "]" + __VA_ARGS__);


#define TINYLOG_DEBUG(...) LOG_HELPER(tiny::LOG_LEVEL::LEVEL_DEBUG,__VA_ARGS__);
#define TINYLOG_ERROR(...) LOG_HELPER(tiny::LOG_LEVEL::LEVLE_ERROR,__VA_ARGS__);
#define TINYLOG_INFO(...) LOG_HELPER(tiny::LOG_LEVEL::LEVEL_INFO,__VA_ARGS__);
#define TINYLOG_WARN(...) LOG_HELPER(tiny::LOG_LEVEL::LEVEL_WARN,__VA_ARGS__);

#define CHECK_NULL(i) \
	if(!i)\
	{ \
		throw std::runtime_error(#i "is null!"); \
	}