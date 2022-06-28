#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

namespace spring::graphics
{
    SpringGraphicsModule::SpringGraphicsModule(core::SpringApplication* app) : SpringModule(app)
    {
        SpringWindow::initialize();
        m_api = SpringGraphicsApi::build();
        m_api->init();
    }

    SpringGraphicsModule::~SpringGraphicsModule()
    {
        for (auto& win : m_windows)
        {
            win.~unique_ptr();
        }
        m_api->shutdown();
        SpringWindow::shutdown();
    }

    void SpringGraphicsModule::update()
    {
        glfwPollEvents();
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
        spdlog::info("Closing!");
    }

    SpringWindow* SpringGraphicsModule::createWindow(WindowDesc desc)
    {
        SpringWindow* window = SpringWindow::build(desc);
        window->setCloseCallback(closeCallback);
        m_windows.emplace_back(window);

        return window;
    }

}