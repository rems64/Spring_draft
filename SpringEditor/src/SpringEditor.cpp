#include <cstdio>
#include <memory>
#include <iostream>
#include <Windows.h>
#include "SpringEditorApplication.hpp"

#include <spdlog/spdlog.h>
#include <Spring/SpringCore/SpringCore.hpp>

#include <Spring/SpringMath/SpringVector3.hpp>

// FOR TESTING ONLY
#include <Spring/SpringCore/SpringProfiler.hpp>

using namespace spring;

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#if defined(SP_WIN32)
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ const int)
{
	{
		SP_START_PROFILING("Spring editor", "debugging.json");

		SpringEditorApplication app({ .showConsole = true, .instance = hInstance});

		app.mainLoop();

		SP_END_PROFILING();
	}

	printf("\n");
	spdlog::info("Application exited successfully!");
#ifndef NDEBUG
	printf("\n");
	spdlog::debug("Keeping the terminal open for you fellow debug user :)");
	std::cin.get();
#endif
	return 0;
}
#elif defined(SP_LINUX)
int main()
{
    {
		SP_START_PROFILING("Spring editor", "debugging.json");

		SpringEditorApplication app({ .showConsole = true, .instance = nullptr});

		app.mainLoop();

		SP_END_PROFILING();
	}

	printf("\n");
	spdlog::info("Application exited successfully!");
#ifndef NDEBUG
	printf("\n");
	spdlog::debug("Keeping the terminal open for you fellow debug user :)");
	std::cin.get();
#endif
	return 0;
}
#endif
