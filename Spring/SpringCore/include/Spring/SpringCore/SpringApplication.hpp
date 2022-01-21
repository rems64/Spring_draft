#pragma once

#include <Spring/SpringCore/SpringModule.hpp>

namespace spring::core
{
    class SpringApplication
    {
    public:
        SpringApplication(HINSTANCE instance);
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

    protected:
        static SpringApplication* m_app;
        HINSTANCE m_instance;
        std::vector<std::unique_ptr<SpringModule>> m_modules;
    };
}