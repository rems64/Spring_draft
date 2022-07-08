#pragma once

#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

using namespace spring;

class SpringEditorApplication : public spring::core::SpringApplication
{
public:
    SpringEditorApplication(spring::core::SpringApplicationInfos info);
    virtual ~SpringEditorApplication();

private:
    Ref<graphics::SpringGraphicsModule> m_graphicsModule;
    Ref<graphics::SpringWindow> m_mainWindow;
    graphics::GraphicsSurface* m_mainWindowSurface;
    graphics::GraphicsDevice* m_mainDevice;
    Ref<graphics::SwapChain> m_swapchain;
    Ref<graphics::Shader> m_vertexShader;
    Ref<graphics::Shader> m_fragmentShader;
};