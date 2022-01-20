#include <Spring/SpringCore/SpringMisc.hpp>

#include <Windows.h>

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
}