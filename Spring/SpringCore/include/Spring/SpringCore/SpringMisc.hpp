#pragma once

namespace spring::core
{
    void info(const char* message, const char* title="Info");
    void error(const char* message, const char* title="Error");


    std::vector<char> readFile(const std::string& filename);
}
