#pragma once

#include <Spring/SpringCore/SpringModule.hpp>
#include <Spring/SpringCore/SpringCommon.hpp>

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
        Ref<T> registerModule()
        {
            Ref<T> module = makeRef<T>(this);
            m_modules.push_back(module);
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
        std::vector<Ref<SpringModule>> m_modules;
    };
}