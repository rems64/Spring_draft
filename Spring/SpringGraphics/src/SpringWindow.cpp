#include <Spring/SpringGraphics/ISpringWindow.hpp>

//#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>
#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>

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

	SpringWindow* SpringWindow::build(WindowDesc desc)
	{
		SpringWindow* window;

#ifdef SE_WINDOWS
		window = new SpringWindow_Glfw(desc); // BAAAAD
#elif GLFW3
		window = makeDesc<SpringWindow_Glfw>(desc);
#else
		spring::core::error("Can't create a window, no supported backend");
		return nullptr;
#endif

		window->construct();
		return window;

	}

	void SpringWindow::initialize()
	{
#ifdef SE_WINDOWS
		if (!glfwInit())
			spdlog::error("Failed to initialize glfw");
		return;
#elif GLFW3
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
#ifdef SE_WINDOWS
		glfwTerminate();
		return;
#elif GLFW3
		glfwTerminate();
		return;
#else
		spring::core::error("Can't shutdown windowing, no supported backend");
		return;
#endif
	}
}