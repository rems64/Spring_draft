#pragma once

#include <Spring/SpringGraphics/SpringWindow.hpp>
#include <>

namespace spring::graphics
{
	class SpringWindow_Glfw
	{
	public:
		SpringWindow_Glfw();
		~SpringWindow_Glfw();

	private:
		GLFWWindow* m_window;
	};
}