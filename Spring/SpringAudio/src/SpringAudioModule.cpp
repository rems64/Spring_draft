#include <Spring/SpringAudio/SpringAudioModule.hpp>

namespace spring::audio
{
	SpringAudioModule::SpringAudioModule(core::SpringApplication* app) : SpringModule(app)
	{
		SP_PROFILE_FUNCTION();
		spdlog::info("Initiliazing Spring Audio");
		m_api = ISpringAudioApi::build();
	}

	SpringAudioModule::~SpringAudioModule() = default;

	void SpringAudioModule::update(bool closeRequired)
	{
		if(closeRequired)
			shouldClose = true;
	}

	bool SpringAudioModule::canClose()
	{
		return shouldClose; // TODO
	}
}
