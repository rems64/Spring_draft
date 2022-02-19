#pragma once

#include <Spring/SpringCore/SpringModule.hpp>

namespace spring::core
{
    struct SpringApplicationInfos
    {
        bool showConsole;
        HINSTANCE instance;
    };

    class SpringApplication
    {
    public:
        SpringApplication(SpringApplicationInfos infos);
        virtual ~SpringApplication() = default;

        template<typename T>
        T* registerModule()
        {
            T* module = new T(this);
            m_modules.push_back(std::unique_ptr<T>(module));
            return module;
        }
        SpringModule* getModule(SpringModuleTypes type);
        static SpringApplication* get() { return m_app; };
        HINSTANCE getNativeInstance() { return m_instance; };

        virtual int mainLoop();

    protected:
        static SpringApplication* m_app;
        HINSTANCE m_instance;
        HWND m_console;
        std::vector<std::unique_ptr<SpringModule>> m_modules;
    };
}