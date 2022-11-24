#ifdef SP_LINUX
#pragma once

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#define GLFW_INCLUDE_NONE
#ifdef SPRING_BUILD_VK
#define GLFW_INCLUDE_VULKAN
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if defined(SP_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(SP_LINUX)
#define GLFW
#endif
#include <GLFW/glfw3native.h>

namespace spring::graphics
{
	struct GraphicsSurface;
	class SpringWindow_Glfw : public SpringWindow
	{
	public:
		explicit SpringWindow_Glfw(WindowDesc desc);
		~SpringWindow_Glfw() override;

		virtual bool construct() override;
		virtual spWinHandle getHandle() override;
		virtual bool buildSurface(SpringGraphicsApi* api) override;
		virtual GraphicsSurface* getSurface() const override;
		virtual bool shouldClose() override;

		virtual void close() override;
#ifdef SPRING_BUILD_VK
		static std::vector<const char*> getRequiredExtensions();
	protected:
		virtual void refreshTheme() override = 0;
#endif
	private:
		GLFWwindow* m_window;
	};
}
#endif