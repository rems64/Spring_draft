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
		virtual nwin getHandle() override;
		virtual SpringSurface getSurface() override;

		LRESULT CALLBACK windowProc(nwin hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		nwin m_window;
	};

	LRESULT CALLBACK winProcDispatch(nwin hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}