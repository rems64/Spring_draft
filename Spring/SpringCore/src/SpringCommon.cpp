#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::core
{
    void info(const char* message, const char* title)
    {
        MessageBoxA(nullptr, message, title, MB_OK);
    }

    void error(const char* message, const char* title)
    {
        MessageBoxA(nullptr, message, title, MB_OK | MB_ICONERROR);
    }

#ifdef SP_WIN32
    spNativeLib loadLibrary(const char* name)
    {
        return LoadLibraryA(name);
    }

#endif
}