#include <Spring/SpringGraphics/SpringWindow_Native.hpp>

#include <Spring/SpringCore/SpringApplication.hpp>

namespace spring::graphics
{
	SpringWindow_Native::SpringWindow_Native()
	{

	}


	SpringWindow_Native::~SpringWindow_Native()
	{

	}

	bool SpringWindow_Native::create()
	{
		const LPCSTR CLASS_NAME = "Sample Window Class";
		WNDCLASS wc = { };

		HINSTANCE hInst = spring::core::SpringApplication::get()->getNativeInstance();

		wc.lpfnWndProc = winProcDispatch;
		wc.hInstance = hInst;
		wc.lpszClassName = CLASS_NAME;
		RegisterClass(&wc);
		m_window = CreateWindowEx(WS_EX_APPWINDOW, CLASS_NAME, this->m_title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);
		SetWindowLongPtr(m_window, 0, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(m_window, SW_SHOWNORMAL);
		return true;
	}

	HWND SpringWindow_Native::getHandle()
	{
		return m_window;
	}

	SpringSurface SpringWindow_Native::getSurface()
	{
		SpringSurface surface
		{
			.handle = m_window,
			.width = m_width,
			.height = m_height
		};
		return surface;
	}

	LRESULT CALLBACK SpringWindow_Native::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			PostQuitMessage(0);
			break;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//std::cout << "Global win proc -> dispatching\n";
		SpringWindow_Native* win = reinterpret_cast<SpringWindow_Native*>(GetWindowLongPtr(hwnd, 0));
		return win->windowProc(hwnd, uMsg, wParam, lParam);
	}
}