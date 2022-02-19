#pragma once

constexpr int SPRING_WINDOW_MAXTITLE_LEN = 40;


#if defined(SE_WINDOWS)
#include <Windows.h>
using nwin = HWND;

#elif defined(SE_LINUX)
#include <X11/Xlib.h>
using nwin = Window;
#else
#error("Spring isn't compatible with your operating system")
#endif

#include <Spring/SpringGraphics/SpringGraphicsResources.hpp>

namespace spring::graphics
{
	struct SpringSurface
	{
		nwin handle;
		uint32_t width;
		uint32_t height;
	};

	class SpringWindow
	{
	public:
		virtual ~SpringWindow() = default;

		virtual bool create() = 0;
		virtual nwin getHandle() = 0;
		virtual SpringSurface getSurface() = 0;
		
		void setTitle(const char* title);
		const char* getTitle();

		void setName(const char* name);
		const char* getName();

		inline uint32_t getWidth() { return m_width; };
		inline uint32_t getHeight() { return m_height; };

		inline SwapChain* getSwapChain() { return &m_swapchain; };

	protected:
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		char* m_name = "Window Name";
		char m_title[SPRING_WINDOW_MAXTITLE_LEN+1] = "Window Title";
		bool m_fullscreen;

		SwapChain m_swapchain;
	};
}