#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>

namespace spring::graphics
{
	SpringWindow_Glfw::SpringWindow_Glfw(WindowDesc desc) : SpringWindow(desc)
	{
		m_window = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
		if (!m_window)
		{
			glfwTerminate();
			spdlog::error("Failed to create glfw window!");
		}
	}

	SpringWindow_Glfw::~SpringWindow_Glfw()
	{

	}

	bool SpringWindow_Glfw::construct()
	{
		return false;
	}

	spWinHandle SpringWindow_Glfw::getHandle()
	{
		return m_window;
	}

	void SpringWindow_Glfw::close()
	{
		glfwDestroyWindow(m_window);
	}

	bool SpringWindow_Glfw::shouldClose()
	{
		return glfwWindowShouldClose(m_window);
	};

	std::vector<const char*> SpringWindow_Glfw::getRequiredExtensions()
	{
		std::vector<const char*> reqExt = {};
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			reqExt.push_back(glfwExtensions[i]);
		}

		return reqExt;
	}
}