#include <Spring/SpringCore/SpringMisc.hpp>

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

    std::vector<char> readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            spring::core::error("Failed to open file!");
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}