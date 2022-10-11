#if defined(_WIN32)

#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>

#include <Spring/SpringCore/SpringApplication.hpp>
#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>
#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>

#if defined(SPRING_BUILD_VK)
#include <Spring/SpringGraphics/SpringGraphicsVulkanUtils.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"
#endif

namespace spring::graphics
{
	SpringWindow_Win32::SpringWindow_Win32(WindowDesc desc) : SpringWindow(desc)
	{

	}


	SpringWindow_Win32::~SpringWindow_Win32()
	{
	}

	bool SpringWindow_Win32::construct()
	{
		const LPCSTR CLASS_NAME = "Sample Window Class";
		WNDCLASS wc = { };

		m_hInst = spring::core::SpringApplication::get()->getNativeInstance();

		wc.lpfnWndProc = winProcDispatch;
		wc.hInstance = m_hInst;
		wc.lpszClassName = CLASS_NAME;
		RegisterClass(&wc);

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		spdlog::info("Screen infos\n  width  ({})\n  height ({})", screenWidth, screenHeight);
		
		if (m_desc.fullscreen)
		{
			if ((m_desc.width != (uint32_t)screenWidth) && (m_desc.height != (uint32_t)screenHeight))
			{
				DEVMODE dmScreenSettings{};
				//memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
				dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
				dmScreenSettings.dmPelsWidth  = m_desc.width;
				dmScreenSettings.dmPelsHeight = m_desc.height;
				dmScreenSettings.dmBitsPerPel = 32;
				dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					{
						m_desc.fullscreen = false;
					}
					else
					{
						return false;
					}
				}
				screenWidth = m_desc.width;
				screenHeight = m_desc.height;
			}

		}
		DWORD dwExStyle;
		DWORD dwStyle;

		if (m_desc.fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}

		RECT windowRect{};
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = m_desc.fullscreen ? (long)screenWidth : (long)m_desc.width;
		windowRect.bottom = m_desc.fullscreen ? (long)screenHeight : (long)m_desc.height;
		
		m_window = CreateWindowEx(0,
			CLASS_NAME,
			this->m_desc.title.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			m_hInst,
			NULL);

		if (m_window == NULL)
		{
			spring::core::error("Failed to create native Win32 window");
			return false;
		}

		if (!m_desc.fullscreen)
		{
			// Center on screen
			uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(m_window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		//SetWindowLongPtr(m_window, 0, reinterpret_cast<LONG_PTR>(this));
		SetPropW(m_window, L"SPRING", this);
		ShowWindow(m_window, SW_SHOWNORMAL);
		return true;
	}

	spWinHandle SpringWindow_Win32::getHandle()
	{
		return &m_window;
	}

	bool SpringWindow_Win32::buildSurface(SpringGraphicsApi* api)
	{
		if(!m_surface)
		{
			const VkInstance* inst = dynamic_cast<SpringGraphicsApi_Vulkan*>(api)->getInstance();
			m_surface = makeRef<GraphicsSurface>();
			Ref<GraphicsSurface_Vulkan> surface_internal = makeRef<GraphicsSurface_Vulkan>();
			surface_internal->relatedInstance = *inst;
			VkWin32SurfaceCreateInfoKHR createInfo =
			{
				.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
				.hinstance = m_hInst,
				.hwnd = m_window,
			};
			VK_CALL(vkCreateWin32SurfaceKHR(*inst, &createInfo, nullptr, &surface_internal->surface));
			m_surface->internal_state = surface_internal;
			m_surface->width = m_desc.width;
			m_surface->height = m_desc.height;
		}

		return true;
	}

	GraphicsSurface* SpringWindow_Win32::getSurface() const
	{
		if(!m_surface)
			core::error("Surface not initialized");
			//buildSurface(dynamic_cast<SpringGraphicsModule*>(core::SpringApplication::get()->getModule(core::SpringModuleTypes::Graphics))->getApi());
		return m_surface.get();
	}

	bool SpringWindow_Win32::shouldClose()
	{
		return m_pendingDestroy;
	}

	void SpringWindow_Win32::close()
	{
		SpringWindow::close();
		m_pendingDestroy = true;
	}

	LRESULT CALLBACK SpringWindow_Win32::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
		{
			int width = LOWORD(lParam);  // Macro to get the low-order word.
			int height = HIWORD(lParam); // Macro to get the high-order word.

			// std::cout << "Resizing window with width(" << width << ") height(" << height << ")\n";
			break;
		}
		case WM_DESTROY:
			close();
			break;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// std::cout << "Global win proc -> dispatching\n";
		// SpringWindow_Win32* win = reinterpret_cast<SpringWindow_Win32*>(GetWindowLongPtr(hwnd, 0));
		SpringWindow_Win32* win = reinterpret_cast<SpringWindow_Win32*>(GetPropW(hwnd, L"SPRING"));
		return win->windowProc(hwnd, uMsg, wParam, lParam);
	}
}

#endif