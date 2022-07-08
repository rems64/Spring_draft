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


#include <Spring/SpringCore/SpringProfiler.hpp>

using namespace spring;

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	{
		SP_START_PROFILING("Spring editor", "debugging.json");

		SpringEditorApplication app({ .showConsole = nCmdShow==0, .instance = hInstance});

		app.mainLoop();

		SP_END_PROFILING();
	}
	
	spdlog::info("Application exited successfully!");
	spdlog::trace("");
#ifndef NDEBUG
	spdlog::debug("Keeping the terminal open for you fellow debug user :)");
	std::cin.get();
#endif
	return 0;
}