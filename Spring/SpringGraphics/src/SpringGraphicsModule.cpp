#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/SpringWindow.hpp>
#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>
#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#include <spdlog/spdlog.h>

namespace spring::graphics
{
    SpringGraphicsModule::SpringGraphicsModule(core::SpringApplication* app) : SpringModule(app)
    {
        m_api = SpringGraphicsApi::build();
        m_api->init();
    }

    SpringGraphicsModule::~SpringGraphicsModule()
    {
        m_api->shutdown();
    }

    void closeCallback(SpringWindow* win)
    {
        spdlog::info("Closing!");
    }

    SpringWindow* SpringGraphicsModule::createWindow(WindowDesc desc)
    {
        Scope<SpringWindow> window = SpringWindow::build(desc);
        SpringWindow* winPtr = window.get();
        window->setCloseCallback(closeCallback);
        m_windows.emplace_back(std::move(window));

        return winPtr;
    }

}