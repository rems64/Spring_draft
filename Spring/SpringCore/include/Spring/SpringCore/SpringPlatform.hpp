#pragma once

#include "SpringCore.hpp"

#if defined(SP_WIN32)
#include <Windows.h>

using spNativeInstance = HINSTANCE;
using spNativeLib = HMODULE;
#endif

namespace spring::core {
    spNativeLib loadLibrary(const char *name);
}