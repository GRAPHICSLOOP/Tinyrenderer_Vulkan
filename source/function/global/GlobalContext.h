#pragma once

#include <memory>
#include "function/render/RenderSystem.h"
#include "function/render/WindowSystem.h"
#include "core/log/LogSystem.h"


namespace tiny
{
	// 管理生命周期，并且创建/销毁所有的全局系统
	class RuntimeGlobalContext
	{
	public:
		void startSystems();
		void shutdownSystems();
	public:
		std::shared_ptr<LogSystem> mLogSystem;
		std::shared_ptr<WindowSystem> mWindowSystem;
		std::shared_ptr<RenderSystem> mRenderSystem;
	};

	extern RuntimeGlobalContext gRuntimeGlobalContext;
}


