#include <Spring/SpringCore/SpringApplication.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>

namespace spring::core
{
    SpringApplication* SpringApplication::m_app = nullptr;
    SpringApplication::SpringApplication(HINSTANCE instance)
    {
        if (!!m_app)
        {
            spring::core::error("Application already exist");
        }
        m_app = this;
        m_instance = instance;
    }
    SpringModule* SpringApplication::getModule(SpringModuleTypes type)
    {
        for (auto& module : m_modules)
        {
            if (module->getType() == type)
            {
                return module.get();
            }
        }

        return nullptr;
    }
}