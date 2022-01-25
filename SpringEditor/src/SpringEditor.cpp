#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

#include <cstdio>
#include <memory>
#include <iostream>
#include <Windows.h>
#include "SpringEditorApplication.hpp"


using namespace spring;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	std::cout << "[DEBUG WINDOW]\n";
	{
		SpringEditorApplication app(hInstance);
		graphics::SpringGraphicsModule* graphicsModule = app.registerModule<graphics::SpringGraphicsModule>();
		graphics::SpringWindow_Native window;
		window.setTitle("Spring Editor");
		window.create();
		graphicsModule->setWindow(&window);

		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				//std::cout << msg.message << std::endl;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
			}
		}
	}

	std::cin.get();
	return 0;
}