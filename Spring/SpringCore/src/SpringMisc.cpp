#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::core
{
    std::vector<char> readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            spdlog::error("Failed to open file ({})", filename.c_str());
            spring::core::error("Failed to open file, please check console for details");
            throw std::runtime_error("failed to open file");
        }
        const size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}