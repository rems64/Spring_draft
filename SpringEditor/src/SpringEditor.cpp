#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

#include <cstdio>
#include <memory>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include "SpringEditorApplication.hpp"

#include <spdlog/spdlog.h>


// FOR TESTING ONLY
#include <vulkan/vulkan.h>
#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>

using namespace spring;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	{
		core::SpringApplicationInfos createInfos{};
		createInfos.instance = hInstance;
		createInfos.showConsole = true;
		SpringEditorApplication app(createInfos);

		graphics::SpringGraphicsModule* graphicsModule = app.registerModule<graphics::SpringGraphicsModule>();

		graphics::SpringWindow* mainWindow = graphicsModule->createWindow({ .title = "Spring Editor" });

		graphics::GraphicsDeviceDesc mainDeviceDesc = {};
		graphics::GraphicsDevice* mainDevice = graphicsModule->getApi()->createDevice({});
		if (!mainWindow)
			return 42;

		spdlog::info("Starting...");

		app.mainLoop();
	}

	spdlog::debug("Keeping the terminal opened for you :)");
	std::cin.get();
	return 0;
}