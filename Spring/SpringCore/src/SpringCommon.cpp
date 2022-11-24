#include <Spring/SpringCore/SpringPlatform.hpp>

namespace spring::core {
#if defined(SP_WIN32)

    void info(const char *message, const char *title) {
        MessageBoxA(nullptr, message, title, MB_OK);
    }

    void error(const char *message, const char *title) {
        MessageBoxA(nullptr, message, title, MB_OK | MB_ICONERROR);
    }
#endif
}