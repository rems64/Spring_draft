#pragma once

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <GLFW/glfw3.h>

namespace spring::graphics
{
	class SpringWindow_Glfw : public SpringWindow
	{
	public:
		SpringWindow_Glfw(WindowDesc desc);
		~SpringWindow_Glfw();

		virtual bool construct() override;
		virtual spWinHandle getHandle() override;
		virtual bool shouldClose() override;

		virtual void close() override;

#ifdef SPRING_BUILD_VK
		static std::vector<const char*> getRequiredExtensions();
#endif
	private:
		GLFWwindow* m_window;
	};
}