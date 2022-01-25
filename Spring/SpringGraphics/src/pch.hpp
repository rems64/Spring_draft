#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <deque>
#include <mutex>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>