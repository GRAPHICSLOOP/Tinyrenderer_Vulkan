#pragma once
#include <spdlog/spdlog.h>

// https://zhuanlan.zhihu.com/p/259667751

namespace tiny
{
    enum class ELogLevel : uint8_t
    {
        debug,
        info,
        warn,
        error
    };

	class LogSystem
	{
	public:
		LogSystem();
		~LogSystem();

		void initialize();

        template<typename... TARGS>
        void log(ELogLevel level, TARGS&&... args)
        {
            switch (level)
            {
            case ELogLevel::debug:
                mLogger->debug(std::forward<TARGS>(args)...);
                break;
            case ELogLevel::info:
                mLogger->info(std::forward<TARGS>(args)...);
                break;
            case ELogLevel::warn:
                mLogger->warn(std::forward<TARGS>(args)...);
                break;
            case ELogLevel::error:
                mLogger->error(std::forward<TARGS>(args)...);
                break;
            default:
                break;
            }
        }

	private:
		std::shared_ptr<spdlog::logger> mLogger;
	};
}

