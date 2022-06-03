#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>
#include "function/render/WindowSystem.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace tiny
{
	struct SwapchainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR mCapabilities;
		vk::SurfaceFormatKHR mFormat;
		vk::PresentModeKHR mPresentMode;
		vk::Extent2D mExtent2D;
		uint32_t mImageCount;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsIndices; // 图形处理队列
		std::optional<uint32_t> presentIndices; // 图形显示队列

		bool isComplete() 
		{
			return graphicsIndices.has_value() && presentIndices.has_value();
		}
	};

	struct VulkanConfigParams
	{
		std::shared_ptr<WindowSystem> mWindowSystem;
	};

	class VulkanRHI
	{
	public:
		~VulkanRHI();
		void initialize(const VulkanConfigParams &params);

	public:
		vk::PhysicalDevice mPhyDevice;
		vk::Device mDevice;
		
		SwapchainSupportDetails mSwapchainSupportDetails;

	private:
		std::array<const char*, 1> mEnableLayerNames = { "VK_LAYER_KHRONOS_validation" };
		QueueFamilyIndices mQueueFamilyIndices;
		std::shared_ptr<WindowSystem> mWindowSystem;
	
	private:
		vk::Instance mInstance;
		VkSurfaceKHR mSurfaceKHR;
		vk::CommandPool mCommandPool;
		vk::CommandBuffer mCommandBuffer;
		vk::SwapchainKHR mSwapchain;
		std::vector<vk::Image> mSwapchainImages;
		std::vector<vk::ImageView> mSwapchainImageViews;

	private:
		void createInstance();
		void createWindowSurface();
		void initializePhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();
		void createCommandBuffers();
		void createDescriptorPool();
		void createSyncPrimitives();
		void createSwapchain();
		void createSwapchainImageViews();
		void createFramebufferImageAndView();

	private:
		void findQueueFamilyIndeices();
		void findSwapchainSupport();

		vk::SurfaceFormatKHR chooseSwapchainSurfaceFormatFromDetails(std::vector<vk::SurfaceFormatKHR> format);
		vk::PresentModeKHR chooseSwapchainPresentModeFromDetails(std::vector<vk::PresentModeKHR> mode);
		vk::Extent2D chooseSwapchainExtentFromDetails(vk::SurfaceCapabilitiesKHR cap);
	};
}


