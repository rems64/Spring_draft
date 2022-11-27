#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#include <Spring/SpringCore/SpringCore.hpp>

#include <Spring/SpringCore/SpringApplication.hpp>

#if defined(SP_LINUX)
#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>
#endif
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
            win.reset();
        }
        SpringWindow::shutdown();
    }

    void SpringGraphicsModule::update(bool closeRequired)
    {
        if (closeRequired)
            shouldClose = true;
#if defined(SP_LINUX)
        SP_PROFILE_FUNCTION();
        {
            SP_PROFILE_SCOPE("glfwPollEvents");
            glfwPollEvents();
        }
#endif
        bool deletion=false;
        for (auto& window : m_windows)
        {
            if (shouldClose || (window && window->shouldClose()))
            {
                deletion = true;
                window = nullptr;
            }
        }
        if (deletion && !anyWindow())
            m_app->shutdown();
    }

    bool SpringGraphicsModule::canClose()
    {
        return !anyWindow();
    }

    void closeCallback(SpringWindow* win)
    {
        SP_PROFILE_FUNCTION();
        spdlog::info("Closing window \"{0}!\"", win->getTitle().c_str());
    }

    Ref<SpringWindow> SpringGraphicsModule::createWindow(WindowDesc desc)
    {
        SP_PROFILE_FUNCTION();
        Ref<SpringWindow> window = SpringWindow::build(std::move(desc));
        window->setCloseCallback(closeCallback);
        m_windows.emplace_back(window);

        return window;
    }

    uint32_t SpringGraphicsModule::getWindowsCount() const
    {
	    return static_cast<uint32_t>(m_windows.size());
    }

    bool SpringGraphicsModule::anyWindow() const
    {
        for (auto& w : m_windows)
            if (w) { return true; }
	    return false;
    }

    SpringGraphicsApi* SpringGraphicsModule::getApi() const
    {
	    return m_api.get();
    }
}
