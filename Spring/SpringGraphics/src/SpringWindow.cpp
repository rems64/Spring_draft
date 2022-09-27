#include <Spring/SpringGraphics/ISpringWindow.hpp>

#include <Spring/SpringCore/SpringProfiler.hpp>

#if defined(SP_WIN32)
#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>
#elif defined(SP_LINUX)
#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>
#endif

namespace spring::graphics
{
	std::string SpringWindow::getTitle()
	{
		return m_desc.title;
	}

	void SpringWindow::setTitle(std::string title)
	{
		m_desc.title = title;
	}

	Ref<SpringWindow> SpringWindow::build(WindowDesc desc)
	{
		Ref<SpringWindow> window;

#ifdef SP_WIN32
		window = makeRef<SpringWindow_Win32>(desc);
#elif SP_LINUX
		window = makeRef<SpringWindow_Glfw>(desc);
#else
		spring::core::error("Can't create a window, no supported backend");
		return nullptr;
#endif

		window->construct();
		return window;

	}

	void SpringWindow::initialize()
	{
		SP_PROFILE_FUNCTION();
#ifdef SP_WIN32
		if (!true)
			spdlog::error("Failed to initialize glfw");
		return;
#elif SP_LINUX
		if (!glfwInit())
			spdlog::error("Failed to initialize glfw");
		return;
#else
		spring::core::error("Can't initialize windowing, no supported backend");
		return;
#endif
	}

	void SpringWindow::shutdown()
	{
#ifdef SP_WIN32
		return;
#elif SP_LINUX
		glfwTerminate();
		return;
#else
		spring::core::error("Can't shutdown windowing, no supported backend");
		return;
#endif
	}
}