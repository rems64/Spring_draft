#include "SpringEditorApplication.hpp"

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

using namespace spring;

float vertices[]  =  {  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f,  0.4f,  0.5f,
                        0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                       -0.5f,  0.5f,   0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f};

SpringEditorApplication::SpringEditorApplication(const spring::core::SpringApplicationInfos infos) : SpringApplication(infos),
                                                                                                        m_swapChain(makeRef<graphics::SwapChain>()),
                                                                                                        m_graphicsPipeline(makeRef<graphics::GraphicsPipeline>()),
                                                                                                        m_vertexShader(makeRef<graphics::Shader>()),
                                                                                                        m_fragmentShader(makeRef<graphics::Shader>()),
                                                                                                        m_commandBuffer(makeRef<graphics::CommandBuffer>()),
                                                                                                        m_commandList(makeRef<graphics::CommandList>()),
                                                                                                        m_vertexBuffer(makeRef<graphics::Buffer>()),
                                                                                                        m_texture(makeRef<graphics::Texture>())
{
    SP_PROFILE_FUNCTION();
	m_graphicsModule = registerModule<spring::graphics::SpringGraphicsModule>();
	m_audioModule = registerModule<spring::audio::SpringAudioModule>();

    // Window creation + surface acquire
	m_mainWindow = m_graphicsModule->createWindow({ .title = "Spring editor", .width = 1280, .height = 720, .fullscreen = false });
	m_mainWindowSurface = m_graphicsModule->getApi()->getSurface(m_mainWindow.get());

    // Create the main device
	m_mainDevice = m_graphicsModule->getApi()->createDevice({ .onlyDiscrete = false, .supportPresent = true, .drawingWindows = { m_mainWindow } });

    // SWAPCHAIN
    // Create the swapchain, linked to the main window
    graphics::SwapChainDesc swapChainDesc =
	{
        .width = m_mainWindow->getWidth(),
        .height = m_mainWindow->getHeight(),
		.presentWindow = true,
		.window = m_mainWindow.get(),
	};
	bool res = m_mainDevice->createSwapChain(swapChainDesc, m_swapChain.get());

    // PIPELINE
    // Vertex shader
	std::vector<char> shader = spring::core::readFile("../../../DevShaders/ColoredTriangle/vert.spv");
	m_mainDevice->createShader({ .stage = graphics::ShaderStage::Vertex, .source = reinterpret_cast<const uint32_t*>(shader.data()), .size = shader.size() }, m_vertexShader.get());
    // Fragment shader
    shader = spring::core::readFile("../../../DevShaders/ColoredTriangle/frag.spv");
	m_mainDevice->createShader({ .stage = graphics::ShaderStage::Fragment, .source = reinterpret_cast<const uint32_t*>(shader.data()), .size = shader.size() }, m_fragmentShader.get());
    // Vertex buffer
    m_mainDevice->createBuffer({ .size = sizeof(vertices) }, m_vertexBuffer.get(), (void*)&vertices[0]);
    // Describing pipeline input layout
    graphics::InputLayout::VertexInput vertexInput = {
            .format = graphics::Format::RGB_32_SFLOAT,
            .inputType = graphics::InputRates::Vertex,
            .binding = 0,
            .location = 0,
            .attributeOffset = 0,
    };
    graphics::InputLayout::VertexInput colorInput = {
            .format = graphics::Format::RGB_32_SFLOAT,
            .inputType = graphics::InputRates::Vertex,
            .binding = 0,
            .location = 1,
            .attributeOffset = 3*sizeof(vertices[0]),
    };
    graphics::InputLayout::VertexInput offsets = {
            .format = graphics::Format::RG_32_SFLOAT,
            .inputType = graphics::InputRates::Vertex,
            .binding = 0,
            .location = 2,
            .attributeOffset = 6*sizeof(vertices[0]),
    };
    graphics::InputLayout layout = {
            .inputs = {
                    vertexInput,
                    colorInput,
                    offsets
            }
    };
	graphics::GraphicsPipelineDesc pipelineDesc = {
            .inputLayout = layout,
            .vs = m_vertexShader.get(),
            .fs = m_fragmentShader.get(),
    };
	m_mainDevice->createGraphicsPipeline(pipelineDesc, m_graphicsPipeline.get());

    // COMMAND BUFFER
    m_mainDevice->createCommandBuffer(m_commandBuffer);
    m_commandList->commandBuffer = m_commandBuffer;

    // TEXTURES
    graphics::TextureDesc texDesc = {
            .format = graphics::Format::RGBA_8_SRGB,
            .usage = graphics::Usage::Default,
            .width = 1024,
            .height = 1024,
            .samples = 1,
            .mipLevels = 0,
    };
    m_mainDevice->createTexture(texDesc, m_texture.get());
}

int SpringEditorApplication::mainLoopCall() {
    SP_PROFILE_FUNCTION();
    m_mainDevice->beginCommandList(m_commandList);
    m_mainDevice->beginRenderPass(m_commandList, m_swapChain);
    m_mainDevice->bindPipeline(m_commandList, m_graphicsPipeline);
    m_mainDevice->validatePipeline(m_commandList);
    m_mainDevice->bindVertexBuffer(m_commandList, m_vertexBuffer, 0);
    m_mainDevice->fastDraw(m_swapChain, m_graphicsPipeline, m_commandList);
    m_mainDevice->endRenderPass(m_commandList);
    m_mainDevice->submitCommandList(m_commandList);
    return 0x0;
}