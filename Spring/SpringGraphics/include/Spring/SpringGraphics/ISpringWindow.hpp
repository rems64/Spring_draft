#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

struct GLFWwindow;


#ifdef SP_WINDOWS
using spWinHandle = GLFWwindow*; // HWND normally, just for temp testing
#elif GLFW3
using spWinHandle = GLFWwindow*;
#else
#error "Can't create window type, unknown OS"
#endif

namespace spring::graphics
{
	class SpringGraphicsApi;
	struct GraphicsSurface;
	struct WindowDesc
	{
		std::string title = "Spring window";
		uint32_t width = 640;
		uint32_t height = 480;
	};

	class SpringWindow
	{
	public:
		SpringWindow(const WindowDesc desc) : m_desc(desc), m_closeCallback(nullptr) {};
		virtual ~SpringWindow() {};

		virtual bool construct() = 0;
		virtual spWinHandle getHandle() = 0;

		virtual void close() { if(m_closeCallback) m_closeCallback(this); };
		virtual bool shouldClose() = 0;
		
		void setTitle(std::string title);
		std::string getTitle();

		uint32_t getWidth() const { return m_desc.width; };
		uint32_t getHeight() const { return m_desc.height; };

		void setCloseCallback(void (*func)(SpringWindow*)) { m_closeCallback = func; };

		static Ref<SpringWindow> build(WindowDesc desc);
		static void shutdown();
		static void initialize();

#ifdef SPRING_BUILD_VK
		static std::vector<const char*> getRequiredExtensions() { return {}; };
#endif
	protected:
		WindowDesc m_desc;
		bool m_fullscreen = false;
		void (*m_closeCallback)(SpringWindow*);
	};
}