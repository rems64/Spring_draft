#pragma once

#ifdef SPRING_BUILD_VK

namespace spring::graphics
{
	constexpr const char* getPhysicalDeviceTypeName(const VkPhysicalDeviceType type)
	{
		switch (type)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return "CPU";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return "Discrete GPU";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return "Integrated GPU";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return "Virtual GPU";
		default:
			return "Other";
		}
	}
}

#define VK_CALL(x) (x)

#endif