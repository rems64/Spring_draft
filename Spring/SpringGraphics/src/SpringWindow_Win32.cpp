#if defined(_WIN32)

#include <Spring/SpringGraphics/SpringWindow_Win32.hpp>

#include <Spring/SpringCore/SpringApplication.hpp>

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

		HINSTANCE hInst = spring::core::SpringApplication::get()->getNativeInstance();

		wc.lpfnWndProc = winProcDispatch;
		wc.hInstance = hInst;
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
			hInst,
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

			// Respond to the message:
			std::cout << "Resizing window with width(" << width << ") height(" << height << ")\n";
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
		//std::cout << "Global win proc -> dispatching\n";
		//SpringWindow_Win32* win = reinterpret_cast<SpringWindow_Win32*>(GetWindowLongPtr(hwnd, 0));
		SpringWindow_Win32* win = reinterpret_cast<SpringWindow_Win32*>(GetPropW(hwnd, L"SPRING"));
		return win->windowProc(hwnd, uMsg, wParam, lParam);
	}
}

#endif