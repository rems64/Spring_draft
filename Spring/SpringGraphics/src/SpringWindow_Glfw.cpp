#include <Spring/SpringCore/SpringCore.hpp>
#if defined(SP_LINUX)

#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>


#ifdef SPRING_BUILD_VK
//#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"
#endif

namespace spring::graphics
{
	SpringWindow_Glfw::SpringWindow_Glfw(WindowDesc desc) : SpringWindow(desc)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // For surface creation!!
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
		SP_PROFILE_FUNCTION();

		glfwDestroyWindow(m_window);
	}

	bool SpringWindow_Glfw::shouldClose()
	{
		SP_PROFILE_FUNCTION();

		return glfwWindowShouldClose(m_window);
	}

	std::vector<const char*> SpringWindow_Glfw::getRequiredExtensions()
	{
		SP_PROFILE_FUNCTION();

		std::vector<const char*> reqExt = {};
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		{
			SP_PROFILE_SCOPE("glfwGetRequiredInstanceExtensions");
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		}
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			reqExt.push_back(glfwExtensions[i]);
		}

		return reqExt;
	}

	bool SpringWindow_Glfw::buildSurface(SpringGraphicsApi* api)
	{
		if(!m_surface)
		{
			const VkInstance* inst = dynamic_cast<SpringGraphicsApi_Vulkan*>(api)->getInstance();
			m_surface = makeRef<GraphicsSurface>();
			Ref<GraphicsSurface_Vulkan> surface_internal = makeRef<GraphicsSurface_Vulkan>();
			surface_internal->relatedInstance = *inst;

			glfwCreateWindowSurface(*inst, m_window, NULL, &surface_internal->surface);
			m_surface->internal_state = surface_internal;
			m_surface->width = m_desc.width;
			m_surface->height = m_desc.height;
		}

		return true;

	}

	GraphicsSurface* SpringWindow_Glfw::getSurface() const
	{
		return m_surface.get();
	}
}
#else
#error "Should not be processed in a non-linux build"
#endif