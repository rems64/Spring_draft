#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <deque>
#include <mutex>
#include <set>
#include <unordered_map>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif


//#define SPRING_BUILD_VK
//#define SPRING_BUILD_GL

#ifdef SPRING_BUILD_VK
//#define VULKAN_HPP_NO_CONSTRUCTORS
//#include <vulkan/vulkan.hpp>
//#include <vulkan/vulkan.h>
#include <volk.h>
#endif
#ifdef SPRING_BUILD_GL
#include <gl/GL.h>
#endif

#include <spdlog/spdlog.h>