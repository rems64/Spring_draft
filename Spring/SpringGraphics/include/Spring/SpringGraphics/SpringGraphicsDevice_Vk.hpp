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
	
	protected:
		VkInstance m_instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
	};
}

#endif