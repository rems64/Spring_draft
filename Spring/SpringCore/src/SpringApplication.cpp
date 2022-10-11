#include <Spring/SpringCore/SpringApplication.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringCore/SpringCore.hpp>

#include <spdlog/spdlog.h>

namespace spring::core
{
    SpringApplication* SpringApplication::m_app = nullptr;
    SpringApplication::SpringApplication(SpringApplicationInfos infos): m_console(NULL)
    {

        if (!!m_app)
        {
            spring::core::error("Application already exist");
        }
        m_app = this;
        m_instance = infos.instance;
        if (infos.showConsole)
        {

            AllocConsole();
            AttachConsole(GetCurrentProcessId());
            
            FILE* fp;
            freopen_s(&fp, "CONIN$", "r", stdin);
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            SetConsoleTitle("Spring console");
            m_console = GetConsoleWindow();
        }

        spdlog::set_pattern("%^[%R] %v%$");
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

    void SpringApplication::shutdown()
    {
        shutdowning = true;
    }

    SpringApplication* SpringApplication::get()
    {
	    return m_app;
    }

    int SpringApplication::mainLoop()
    {
        SP_PROFILE_FUNCTION();
#if defined(SP_WIN32)
        MSG msg;
        bool systemClose = false;
        bool close = false;
        while (!close)
        {
		    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			    TranslateMessage(&msg);
			    DispatchMessage(&msg);
			    if (msg.message == WM_QUIT) {
				    systemClose = true;
				    break;
			    }
		    }
            bool modulesClose = true;
            for (const auto& module : m_modules)
            {
                module->update(systemClose || shutdowning);
                modulesClose &= module->canClose();
            }
            close = (modulesClose && systemClose) || modulesClose;
        }
#endif
        return 0;
    }
}