#pragma once

#include "SpringCore.hpp"


#if defined(SP_WIN32)
#include <Windows.h>
using spNativeInstance = HINSTANCE;
using spNativeLib = HMODULE;

#elif defined(SP_LINUX)
using spNativeInstance = void*;
using spNativeLib = void*;
#endif

namespace spring::core {
    spNativeLib loadLibrary(const char *name);
}