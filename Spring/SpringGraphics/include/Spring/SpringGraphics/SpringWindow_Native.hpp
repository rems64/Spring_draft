#pragma once

#include <Spring/SpringGraphics/SpringWindow.hpp>

namespace spring::graphics
{
	class SpringWindow_Native : public SpringWindow
	{
	public:
		SpringWindow_Native();
		virtual ~SpringWindow_Native() override;

		virtual bool create() override;
		virtual HWND getHandle() override;

		LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND m_window;
	};

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}