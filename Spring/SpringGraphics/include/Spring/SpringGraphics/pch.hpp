#pragma once

#ifdef _WIN32
#define SE_WINDOWS
#else
#define SE_LINUX
#endif

#ifdef SE_WINDOWS
#define NOMINMAX
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef SPRING_BUILD_VK
//#include <vulkan/vulkan.h> // To fix
#endif

#include <algorithm>
#include <spdlog/spdlog.h>
#include <deque>
#include <array>