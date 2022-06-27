#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

#ifdef SE_WINDOWS
using spWinHandle = HWND;
#elif GLFW3
using spWinHandle = GLFWWindow*;
#else
#error "Can't create window type, unknown OS"
#endif

namespace spring::graphics
{
	struct WindowDesc
	{
		std::string title = "Spring window";
		uint32_t width = 640;
		uint32_t height = 480;
	};

	class SpringWindow
	{
	public:
		SpringWindow(WindowDesc desc) : m_desc(desc) {};
		virtual ~SpringWindow() {};

		virtual bool construct() = 0;
		virtual spWinHandle getHandle() = 0;

		virtual void close() { if(m_closeCallback) m_closeCallback(this); };
		
		void setTitle(std::string title);
		std::string getTitle();

		inline uint32_t getWidth() { return m_desc.width; };
		inline uint32_t getHeight() { return m_desc.height; };

		void setCloseCallback(void (*func)(SpringWindow*)) { m_closeCallback = func; };

		static Scope<SpringWindow> build(WindowDesc desc);

	protected:
		WindowDesc m_desc;
		bool m_fullscreen = false;
		void (*m_closeCallback)(SpringWindow*);
	};
}