#if defined(_WIN32)

#pragma once

#include <Spring/SpringGraphics/ISpringWindow.hpp>

namespace spring::graphics
{
	class SpringWindow_Win32 : public SpringWindow
	{
	public:
		SpringWindow_Win32(WindowDesc desc);
		~SpringWindow_Win32() override;

		virtual bool construct() override;
		virtual spWinHandle getHandle() override;
		virtual bool shouldClose() override;

		virtual void close() override;

		LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND m_window;
		bool m_pendingDestroy = false;
	};

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

#endif