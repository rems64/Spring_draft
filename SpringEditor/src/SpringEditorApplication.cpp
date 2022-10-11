#include "SpringEditorApplication.hpp"

#include <Spring/SpringCore/SpringCommon.hpp>
#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

using namespace spring;

SpringEditorApplication::SpringEditorApplication(const spring::core::SpringApplicationInfos infos) : SpringApplication(infos), m_swapChain(makeRef<graphics::SwapChain>())
//SpringEditorApplication::SpringEditorApplication(const spring::core::SpringApplicationInfos infos) : SpringApplication(infos)
{
	m_graphicsModule = registerModule<spring::graphics::SpringGraphicsModule>();
	//m_audioModule = registerModule<spring::audio::SpringAudioModule>();
	m_mainWindow = m_graphicsModule->createWindow({ .title = "Spring editor", .width = 1280, .height = 720, .fullscreen = false });
	m_mainWindowSurface = m_graphicsModule->getApi()->getSurface(m_mainWindow.get());
	
	m_mainDevice = m_graphicsModule->getApi()->createDevice({ .onlyDiscrete = false, .supportPresent = true, .drawingWindows = { m_mainWindow } });
	graphics::SwapChainDesc swapChainDesc =
	{
		.presentWindow = true,
		.window = m_mainWindow.get()
	};

	bool res = m_mainDevice->createSwapChain(swapChainDesc, m_swapChain.get());
	if (!res)
		SPRING_ERROR("Failed to create swap chain!")

	//m_mainDevice->createShader(graphics::ShaderStage::Vertex, );
	//std::vector<char> shader = spring::core::readFile("shaders/test");
}