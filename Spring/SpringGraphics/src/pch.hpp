#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <deque>
#include <mutex>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif


#define SPRING_BUILD_VK
#define SPRING_BUILD_GL

#ifdef SPRING_BUILD_VK
#define VULKAN_HPP_NO_CONSTRUCTORS
//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#endif
#ifdef SPRING_BUILD_GL
#include <gl/GL.h>
#endif

#include <spdlog/spdlog.h>