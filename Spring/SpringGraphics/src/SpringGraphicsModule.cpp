#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

namespace spring::graphics
{
    SpringGraphicsModule::SpringGraphicsModule(core::SpringApplication* app) : SpringModule(app)
    {
        SP_PROFILE_FUNCTION();
        SpringWindow::initialize();
        m_api = SpringGraphicsApi::build();
        m_api->init();
    }

    SpringGraphicsModule::~SpringGraphicsModule()
    {
        SP_PROFILE_FUNCTION();
        for (auto& win : m_windows)
        {
            win.~shared_ptr();
        }
        m_api->~SpringGraphicsApi();
        SpringWindow::shutdown();
    }

    void SpringGraphicsModule::update()
    {
        SP_PROFILE_FUNCTION();
        {
            SP_PROFILE_SCOPE("glfwPollEvents");
            glfwPollEvents();
        }
        bool deletion=false;
        for (auto& window : m_windows)
        {
            if (window->shouldClose())
            {
                deletion = true;
                window->close();
            }
        }
        if(deletion)
            m_windows.~vector();
        return;
    }

    bool SpringGraphicsModule::canClose()
    {
        return !anyWindow();
    }

    void closeCallback(SpringWindow* win)
    {
        SP_PROFILE_FUNCTION();
        spdlog::info("Closing!");
    }

    Ref<SpringWindow> SpringGraphicsModule::createWindow(WindowDesc desc)
    {
        SP_PROFILE_FUNCTION();
        Ref<SpringWindow> window = SpringWindow::build(desc);
        window->setCloseCallback(closeCallback);
        m_windows.emplace_back(window);

        return window;
    }

}