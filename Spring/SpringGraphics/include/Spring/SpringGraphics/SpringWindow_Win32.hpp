#pragma once

#include <Spring/SpringGraphics/SpringWindow.hpp>

namespace spring::graphics
{
	class SpringWindow_Win32 : public SpringWindow
	{
	public:
		SpringWindow_Win32(WindowDesc desc);
		virtual ~SpringWindow_Win32() override;

		virtual bool construct() override;
		virtual spWinHandle getHandle() override;

		virtual void close() override;

		LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND m_window;
	};

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}