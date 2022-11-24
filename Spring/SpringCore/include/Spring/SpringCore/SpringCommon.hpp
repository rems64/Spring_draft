#pragma once

#define SPRING_ERROR(text) {spdlog::error(text);throw std::runtime_error(text);}

namespace spring::core
{
    void info(const char* message, const char* title = "Info");
    void error(const char* message, const char* title = "Error");
}