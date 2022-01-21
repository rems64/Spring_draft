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

    bool SpringGraphicsModule::setWindow(HWND handle)
    {
        return true;
    }
}