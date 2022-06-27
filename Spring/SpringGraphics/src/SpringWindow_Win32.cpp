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
		m_window = CreateWindowEx(WS_EX_APPWINDOW, CLASS_NAME, this->m_desc.title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);
		SetWindowLongPtr(m_window, 0, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(m_window, SW_SHOWNORMAL);
		return true;
	}

	spWinHandle SpringWindow_Win32::getHandle()
	{
		return m_window;
	}

	void SpringWindow_Win32::close()
	{
		//SpringWindow::close();
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
			//std::cout << "Resizing window with width(" << width << ") height(" << height << ")\n";
			break;
		}
		case WM_DESTROY:
			//PostQuitMessage(0);
			close();
			break;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//std::cout << "Global win proc -> dispatching\n";
		SpringWindow_Win32* win = reinterpret_cast<SpringWindow_Win32*>(GetWindowLongPtr(hwnd, 0));
		return win->windowProc(hwnd, uMsg, wParam, lParam);
	}
}