#include "RenderSystem.h"
#include "function/global/GlobalContext.h"

void tiny::RenderSystem::initialize()
{
	tiny::VulkanConfigParams vulkanParams;
	vulkanParams.mWindowSystem = tiny::gRuntimeGlobalContext.mWindowSystem;

	mVulkanRHI = std::make_shared<VulkanRHI>();
	mVulkanRHI->initialize(vulkanParams);
}
