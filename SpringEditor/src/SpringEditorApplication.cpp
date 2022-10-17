#include "SpringEditorApplication.hpp"

#include <Spring/SpringCore/SpringCommon.hpp>
#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

using namespace spring;

SpringEditorApplication::SpringEditorApplication(const spring::core::SpringApplicationInfos infos) : SpringApplication(infos), m_swapChain(makeRef<graphics::SwapChain>()), m_graphicsPipeline(makeRef<graphics::GaphicsPipeline>()), m_vertexShader(makeRef<graphics::Shader>())
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

	std::vector<char> shader = spring::core::readFile("../../../DevShaders/ColoredTriangle/vert.spv");
	m_mainDevice->createShader({ .stage = graphics::ShaderStage::Vertex, .source = reinterpret_cast<const uint32_t*>(shader.data()), .size = shader.size() }, m_vertexShader.get());

	graphics::GraphicsPipelineDesc pipelineDesc = {};
	m_mainDevice->createGraphicsPipeline(pipelineDesc, m_graphicsPipeline.get());


}