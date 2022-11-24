#include <Spring/SpringCore/SpringPlatform.hpp>

namespace spring::core {
#if defined(SP_WIN32)
    spNativeLib loadLibrary(const char *name) {
        return LoadLibraryA(name);
    }
#elif defined(SP_LINUX)
    #include <stdio.h>
    #include <stdlib.h>
    #include <dlfcn.h>
    spNativeLib loadLibrary(const char *name) {
        return dlopen(name, RTLD_LAZY);
    }
#endif
}
