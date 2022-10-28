#pragma once

#if defined(_WIN32)
#ifndef SP_WIN32
#define SP_WIN32
#endif
#elif defined(__linux__) || defined(__FreeBSD__)
#ifndef SP_LINUX
#define SP_LINUX
#endif
#endif

//#if defined(SP_WIN32)
//using spNativeLib = HMODULE;
//#endif



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

#ifdef SPRING_PROFILE
#include "SpringProfiler.hpp"
#define SP_START_PROFILING(name, path) spring::core::Profiler::get().beginProfiling(name, path)
#define SP_END_PROFILING() spring::core::Profiler::get().endProfiling()
//#define _SP_PROFILE_SCOPE_LINE(name, line) constexpr auto fixedName##line = spring::core::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
//																			spring::core::InstrumentationTimer timer##line(fixedName##line.Data)
#define _SP_PROFILE_SCOPE_LINE(name, line) spring::core::InstrumentationTimer timer##line(name)
#define SP_PROFILE_SCOPE_LINE(name, line) _SP_PROFILE_SCOPE_LINE(name, line)
#define SP_PROFILE_SCOPE(name) SP_PROFILE_SCOPE_LINE(name, __LINE__)
#define SP_PROFILE_FUNCTION() SP_PROFILE_SCOPE(__func__)
#else
#define SP_START_PROFILING(name, path) do {} while(0)
#define SP_END_PROFILING() do {} while(0)
#define SP_PROFILE_SCOPE_LINE(name, line) do {} while(0)
#define SP_PROFILE_SCOPE(name) do {} while(0)
#define SP_PROFILE_FUNCTION() do {} while(0)
#endif