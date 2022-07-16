#pragma once

#ifdef SP_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef SPRING_BUILD_VK
//#include <vulkan/vulkan.h> // To fix
#endif

#include <algorithm>
#include <spdlog/spdlog.h>
#include <deque>
#include <array>