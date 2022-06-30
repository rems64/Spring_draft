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
#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>

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

		graphics::GraphicsSurface* surface = mainWindow->getSurface(graphicsModule->getApi());
		graphics::GraphicsDevice* mainDevice = graphicsModule->getApi()->createDevice({ .surfaces = { surface }});
		graphics::SwapChain swapchain = {};
		graphics::SwapChainDesc swapChainDesc =
		{
			.width = 640,
			.height = 480,
			.hasSurface = true,
			.surface = surface
		};
		mainDevice->createSwapChain(swapChainDesc, &swapchain);
		if (!mainWindow)
			return 42;

		app.mainLoop();
	}
	
	spdlog::info("Application exited successfully!");
	spdlog::trace("");
	spdlog::debug("Keeping the terminal open for you :)");
	std::cin.get();
	return 0;
}