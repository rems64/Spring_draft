#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

#include <cstdio>
#include <memory>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include "SpringEditorApplication.hpp"

#include <spdlog/spdlog.h>

using namespace spring;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	core::SpringApplicationInfos createInfos{};
	createInfos.instance = hInstance;
	createInfos.showConsole = true;
	SpringEditorApplication app(createInfos);

	graphics::SpringGraphicsModule* graphicsModule = app.registerModule<graphics::SpringGraphicsModule>();

	graphics::SpringWindow* mainWindow = graphicsModule->createWindow({ .title = "Spring Editor" });
	if (!mainWindow)
		return 42;
	
	spdlog::info("Starting...");

	spdlog::warn("Any window : {}", graphicsModule->anyWindow());

	while (true)
	{
	};
	//while(true) {}

	spdlog::warn("Any window : {}", graphicsModule->anyWindow());

	spdlog::debug("Keeping the terminal opened for you :)");
	std::cin.get();
	return 0;
}