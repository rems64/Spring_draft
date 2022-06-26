#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

#include <cstdio>
#include <memory>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include "SpringEditorApplication.hpp"


using namespace spring;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	core::SpringApplicationInfos createInfos{};
	createInfos.instance = hInstance;
	createInfos.showConsole = true;
	SpringEditorApplication app(createInfos);

	graphics::SpringGraphicsModule* graphicsModule = app.registerModule<graphics::SpringGraphicsModule>();

	graphics::SpringWindow* mainWindow = graphicsModule->createWindow("Spring Editor");
	if (!mainWindow)
		return -1;
	graphicsModule->createSwapChain(mainWindow);
	
	
	graphicsModule->tmpSetupRender();

	//app.mainLoop();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		graphicsModule->tmpFrame();
	};
	//while(true) {}

	return 0;
}