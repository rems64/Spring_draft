#pragma once

#ifdef SPRING_BUILD_VK

namespace spring::graphics
{
	constexpr const char* getPhysicalDeviceTypeName(VkPhysicalDeviceType type)
	{
		switch (type)
		{
		case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
			return "CPU";
		case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return "Discrete GPU";
		case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return "Integrated GPU";
		case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return "Virtual GPU";
		default:
			return "Other";
		}
	}
}

#endif