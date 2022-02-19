#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/SpringGraphicsDevice_Vk.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringGraphics/SpringWindow.hpp>
#include <Spring/SpringGraphics/SpringWindow_Native.hpp>

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        spring::core::error("Failed to open file!");
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}


namespace spring::graphics
{
    SpringGraphicsModule::SpringGraphicsModule(core::SpringApplication* app) : SpringModule(app)
    {
        m_device = std::make_unique<graphics::Device_Vulkan>();
    }

    SpringGraphicsModule::~SpringGraphicsModule()
    {
    }

    SpringWindow* SpringGraphicsModule::createWindow(const char* title, SpringWindowTypes type)
    {
        std::shared_ptr<SpringWindow> window;
        switch (type)
        {
        case SpringWindowTypes::Native:
            window = std::make_shared<SpringWindow_Native>();
            window->setTitle(title);
            window->create();
            m_windows.emplace_back(window);
            break;
        case SpringWindowTypes::None:
            spring::core::error("Can't create a window from None");
            return nullptr;
        default:
            spring::core::error("Can't create a window");
            return nullptr;
        }
        return window.get();
    }

    bool SpringGraphicsModule::createSwapChain(SpringWindow* window)
    {
        SwapChainDesc desc = {};
        desc.fullscreen = false;
        desc.width = window->getWidth();
        desc.height = window->getHeight();
        desc.vsync = true;
        m_device->createSwapChain(window->getSwapChain(), desc, window->getSurface());
        return true;
    }

    void SpringGraphicsModule::tmpSetupRender()
    {
        auto vertSource = readFile("../../../DevShaders/ColoredTriangle/vert.spv");
        auto fragSource = readFile("../../../DevShaders/ColoredTriangle/frag.spv");
        m_device->createShader(&vs, ShaderStages::VertexShader, vertSource.data(), vertSource.size());
        m_device->createShader(&fs, ShaderStages::FragmentShader, fragSource.data(), fragSource.size());
        PipelineStateDesc pipelineDesc = {};
        pipelineDesc.vertexShader = &vs;
        pipelineDesc.fragmentShader = &fs;
        m_device->createPipelineState(&pipelineState, &pipelineDesc);
        //RenderPassDesc renderPassDesc;
        //renderPassDesc.attachments = [];
        //m_device->createRenderPass(&renderPass, renderPassDesc);
        cmd = m_device->beginCommandList(m_windows[0]->getSwapChain());
        m_device->bindPipelineState(&pipelineState, cmd);
        //m_device->bindRenderPass(pipelineState.data)
        m_device->beginRenderPass(m_windows[0]->getSwapChain(), &cmd);
    }

    void SpringGraphicsModule::tmpFrame()
    {
        m_device->renderFrame(m_windows[0]->getSwapChain(), &cmd);
    }
}