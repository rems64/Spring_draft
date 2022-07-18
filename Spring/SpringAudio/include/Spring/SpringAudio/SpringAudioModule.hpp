#pragma once

#include <Spring/SpringCore/SpringModule.hpp>
#include <Spring/SpringAudio/ISpringAudioApi.hpp>

namespace spring::audio
{
	class SpringAudioModule : public core::SpringModule
	{
	public:
		explicit SpringAudioModule(core::SpringApplication* app);
		~SpringAudioModule() override;

		void update() override;
		bool canClose() override;
	protected:
		Scope<ISpringAudioApi> m_api;
	};
}