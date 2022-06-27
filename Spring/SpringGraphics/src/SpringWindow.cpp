#include <Spring/SpringGraphics/SpringWindow.hpp>

#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>
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

	Scope<SpringWindow> SpringWindow::build(WindowDesc desc)
	{
		Scope<SpringWindow> window;

#ifdef SE_WINDOWS
		window = makeScope<SpringWindow_Win32>(desc);
#elif GLFW3
		window = makeDesc<SpringWindow_Glfw3>(desc);
#else
		spring::core::error("Can't create a window, no supported backend");
		return nullptr;
#endif

		window->construct();
		return window;

	}
}