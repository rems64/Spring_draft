#include <Spring/SpringGraphics/SpringGraphicsModule.hpp>

#include <Spring/SpringGraphics/SpringGraphicsDevice_Vk.hpp>

namespace spring::graphics
{
    SpringGraphicsModule::SpringGraphicsModule(core::SpringApplication* app) : SpringModule(app)
    {
        m_device = std::make_unique<graphics::Device_Vulkan>();
    }

    SpringGraphicsModule::~SpringGraphicsModule()
    {
    }

    bool SpringGraphicsModule::setWindow(SpringWindow* window)
    {
        SwapChainDesc desc = {};
        desc.fullscreen = false;
        desc.width = 1280;
        desc.height = 720;
        desc.vsync = true;
        m_device->createSwapChain(&m_swapchain, desc, window);
        return true;
    }
}