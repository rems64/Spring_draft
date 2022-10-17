#pragma once

#include <Spring/SpringCore/SpringCore.hpp>
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
	 * @param infos the needed informations for creating the app, can be default
	 */
	explicit SpringEditorApplication(const spring::core::SpringApplicationInfos infos);

private:
    Ref<spring::graphics::SpringGraphicsModule> m_graphicsModule;
    Ref<spring::audio::SpringAudioModule> m_audioModule;
    Ref<spring::graphics::SpringWindow> m_mainWindow;
    spring::graphics::GraphicsSurface* m_mainWindowSurface;
    spring::graphics::GraphicsDevice* m_mainDevice;
    Ref<spring::graphics::SwapChain> m_swapChain;
    Ref<spring::graphics::Shader> m_vertexShader;
    Ref<spring::graphics::Shader> m_fragmentShader;
    Ref<spring::graphics::GaphicsPipeline> m_graphicsPipeline;
};