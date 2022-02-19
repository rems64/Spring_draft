#include <Spring/SpringCore/SpringApplication.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>

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

    int SpringApplication::mainLoop()
    {
        MSG msg = { 0 };
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            //graphicsModule->tmpFrame();
        }
        return 0;
    }
}