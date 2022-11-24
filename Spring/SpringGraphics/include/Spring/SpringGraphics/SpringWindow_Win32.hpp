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
		virtual bool buildSurface(SpringGraphicsApi* api) override;
		virtual GraphicsSurface* getSurface() const override;
		virtual bool shouldClose() override;

		virtual void close() override;

		LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		virtual void refreshTheme() override;
	protected:
		HWND m_window = NULL;
		HINSTANCE m_hInst = NULL;
		bool m_pendingDestroy = false;
	};

	LRESULT CALLBACK winProcDispatch(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

#else
#error "Should not be included in a non-windows build"
#endif