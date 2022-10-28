#include <Spring/SpringCore/SpringPlatform.hpp>

namespace spring::core
{
    spNativeLib loadLibrary(const char *name) {
        return LoadLibraryA(name);
    }
}
