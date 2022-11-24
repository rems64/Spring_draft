#pragma once

#include <Spring/SpringCore/SpringApplication.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>
#include <Spring/SpringAudio/SpringAudio.hpp>

/**
 * \brief Editor application class, manages the app through it's lifetime
 */
class SpringEditorApplication final : public spring::core::SpringApplication
{
public:
	/**
	 * @brief Construct app, initialize graphics module
	 * @param infos the needed pieces of information for creating the app
	 */
	explicit SpringEditorApplication(spring::core::SpringApplicationInfos infos);

    int mainLoopCall() override;

private:
    Ref<spring::graphics::SpringGraphicsModule> m_graphicsModule;
    Ref<spring::audio::SpringAudioModule> m_audioModule;
    Ref<spring::graphics::SpringWindow> m_mainWindow;
    spring::graphics::GraphicsSurface* m_mainWindowSurface;
    spring::graphics::GraphicsDevice* m_mainDevice;
    Ref<spring::graphics::SwapChain> m_swapChain;
    Ref<spring::graphics::Shader> m_vertexShader;
    Ref<spring::graphics::Shader> m_fragmentShader;
    Ref<spring::graphics::GraphicsPipeline> m_graphicsPipeline;
    Ref<spring::graphics::CommandBuffer> m_commandBuffer;
    Ref<spring::graphics::CommandList> m_commandList;
    Ref<spring::graphics::Buffer> m_vertexBuffer;
};