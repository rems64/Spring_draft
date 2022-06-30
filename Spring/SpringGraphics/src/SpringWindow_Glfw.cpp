#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>

#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>
#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#ifdef SPRING_BUILD_VK
#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
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

	GraphicsSurface* SpringWindow_Glfw::getSurface(SpringGraphicsApi* api)
	{
		Scope<GraphicsSurface> surface = makeScope<GraphicsSurface>();
#ifdef SPRING_BUILD_VK
		SpringGraphicsApi_Vulkan* api_vk = static_cast<SpringGraphicsApi_Vulkan*>(api);
		Ref<GraphicsSurface_Vulkan> surface_internal = makeRef<GraphicsSurface_Vulkan>();
		VkResult res = glfwCreateWindowSurface(*api_vk->getInstance(), m_window, nullptr, &surface_internal->surface);
		if (res != VK_SUCCESS)
		{
			const char* description;
			int code = glfwGetError(&description);

			if (description)
				spdlog::error("code: {}, {}", code, description);
			spdlog::error("Error while creating window surface! ({})", res);
		}
		surface_internal->relatedInstance = *api_vk->getInstance();
		surface->internal_state = surface_internal;
		GraphicsSurface* surfacePtr = surface.get();
		api_vk->registerSurface(surface);
#endif
		return surfacePtr;
	}
}