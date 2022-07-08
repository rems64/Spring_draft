#include "SpringEditorApplication.hpp"

#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

SpringEditorApplication::SpringEditorApplication(spring::core::SpringApplicationInfos infos) : SpringApplication(infos), m_swapchain(makeRef<graphics::SwapChain>())
{
	bool res=true;
	m_graphicsModule = registerModule<graphics::SpringGraphicsModule>();
	m_mainWindow = m_graphicsModule->createWindow({ .title = "Spring editor" });
	m_mainWindowSurface = m_graphicsModule->getApi()->getSurface(m_mainWindow.get());
	
	m_mainDevice = m_graphicsModule->getApi()->createDevice({ .surfaces = { m_mainWindowSurface } });
	graphics::SwapChainDesc scdesc =
	{
		.hasSurface = true,
		.surface = m_mainWindowSurface
	};
	res = m_mainDevice->createSwapChain(scdesc, m_swapchain.get());
	if (!res)
		SPRING_ERROR("Failed to create swap chain!");

	//m_mainDevice->createShader(graphics::ShaderStage::Vertex, );
	std::vector<char> shader = spring::core::readFile("shaders/test");
}

SpringEditorApplication::~SpringEditorApplication()
{

}