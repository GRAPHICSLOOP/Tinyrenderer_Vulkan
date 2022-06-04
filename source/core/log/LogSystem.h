#pragma once
#include <spdlog/spdlog.h>

// https://zhuanlan.zhihu.com/p/259667751

namespace tiny
{
    enum class LOG_LEVEL : uint8_t
    {
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARN,
        LEVEL_ERROR
    };

	class LogSystem
	{
	public:
		LogSystem();
		~LogSystem();

		void initialize();

        template<typename... TARGS>
        void log(LOG_LEVEL level, TARGS&&... args)
        {
            switch (level)
            {
            case LOG_LEVEL::LEVEL_DEBUG:
                mLogger->debug(std::forward<TARGS>(args)...);
                break;
            case LOG_LEVEL::LEVEL_INFO:
                mLogger->info(std::forward<TARGS>(args)...);
                break;
            case LOG_LEVEL::LEVEL_WARN:
                mLogger->warn(std::forward<TARGS>(args)...);
                break;
            case LOG_LEVEL::LEVEL_ERROR:
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

