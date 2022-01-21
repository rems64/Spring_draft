#pragma once
#define SPRING_BUILD_VK

#ifdef SPRING_BUILD_VK

#include "SpringGraphicsDevice_Vk.hpp"
#include "SpringGraphicsDevice.hpp"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

namespace spring::graphics
{
	class Device_Vulkan : public Device
	{
	public:
		Device_Vulkan();
		virtual ~Device_Vulkan() override;

		virtual int createSwapChain(SwapChain* swapchain, SwapChainDesc swapchainDesc, spring::graphics::SpringWindow* window);

	protected:
		VkInstance m_instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	};
}

#endif