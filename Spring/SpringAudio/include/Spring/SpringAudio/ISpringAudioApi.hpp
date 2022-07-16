#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::audio
{
	class ISpringAudioApi
	{
	public:
		//ISpringAudioApi() = default;

		//ISpringAudioApi(ISpringAudioApi&) = delete;
		//ISpringAudioApi(ISpringAudioApi&&) = delete;
		//ISpringAudioApi& operator=(const ISpringAudioApi&) = delete;
		//ISpringAudioApi& operator=(const ISpringAudioApi&&) = delete;

		//virtual ~ISpringAudioApi() = 0;

		static Scope<ISpringAudioApi> build();
	};

}