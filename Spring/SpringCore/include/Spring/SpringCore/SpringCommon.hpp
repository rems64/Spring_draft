#pragma once

#include <Spring/SpringCore/SpringCore.hpp>

//#if defined(_WIN32)
//#ifndef SP_WIN32
//#define SP_WIN32
//#endif
//#elif defined(__linux__) || defined(__FreeBSD__)
//#define SP_LINUX
//#endif
//#ifdef SP_WIN32
//#define NOMINMAX
////#include <Windows.h>
//using spNativeHandle = HWND; // HWND normally, just for temp testing
//using spNativeLib = HMODULE;
//#elif SP_LINUX
//#error "Linux support is on it's way..."
//#else
//#error "Can't create window type, unknown OS"
//#endif

#define SPRING_ERROR(text) {spdlog::error(text);throw std::runtime_error(text);}

namespace spring::core
{
    void info(const char* message, const char* title = "Info");
    void error(const char* message, const char* title = "Error");
}