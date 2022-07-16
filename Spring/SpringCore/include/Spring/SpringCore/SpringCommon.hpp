#pragma once

#ifdef _WIN32
#define SP_WINDOWS
#endif

#ifdef SP_WINDOWS
#define NOMINMAX
#include <Windows.h>
using spNativeHandle = HWND; // HWND normally, just for temp testing
using spNativeLib = HMODULE;
#elif SP_LINUX
#error "Linux support is on it's way..."
#else
#error "Can't create window type, unknown OS"
#endif

template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename ... Args>
constexpr Scope<T> makeScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}


template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Ref<T> makeRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

#define SPRING_ERROR(text) {spdlog::error(text);throw std::runtime_error(text);}

namespace spring::core
{
    void info(const char* message, const char* title = "Info");
    void error(const char* message, const char* title = "Error");

	spNativeLib loadLibrary(const char* name);
}