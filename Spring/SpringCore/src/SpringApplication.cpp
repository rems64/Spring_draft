#include <Spring/SpringCore/SpringApplication.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringCore/SpringCore.hpp>

#include <spdlog/spdlog.h>

namespace spring::core
{
    SpringApplication* SpringApplication::m_app = nullptr;
    SpringApplication::SpringApplication(SpringApplicationInfos infos)
    {

        if (!!m_app)
        {
            spring::core::error("Application already exist");
        }
        m_app = this;
        m_instance = infos.instance;
        if (infos.showConsole)
        {
            FILE* fp;

            AllocConsole();
            SetConsoleTitle("Spring console");
            freopen_s(&fp, "CONIN$", "r", stdin);
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            m_console = GetConsoleWindow();
        }

        spdlog::set_pattern("%^%v%$");
        spdlog::set_level(spdlog::level::trace);
        //spdlog::default_logger()->flush_on(spdlog::level::debug);
    }

    SpringModule* SpringApplication::getModule(const SpringModuleTypes type) const
    {
        for (const auto& module : m_modules)
        {
            if (module->getType() == type)
            {
                return module.get();
            }
        }

        return nullptr;
    }

    HINSTANCE SpringApplication::getNativeInstance() const
    {
	    return m_instance;
    }

    SpringApplication* SpringApplication::get()
    {
	    return m_app;
    }

    int SpringApplication::mainLoop()
    {
        SP_PROFILE_FUNCTION();
        bool close = false;
        while (!close)
        {
            close = true;
            for (const auto& module : m_modules)
            {
                module->update();
                close &= module->canClose();
            }
        }
        return 0;
    }
}