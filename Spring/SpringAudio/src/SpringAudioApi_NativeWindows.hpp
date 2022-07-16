#pragma once

#include <Spring/SpringAudio/ISpringAudioApi.hpp>

namespace spring::audio
{
	class SpringAudioApi_NativeWindows final : public ISpringAudioApi
	{
	public:
		SpringAudioApi_NativeWindows();
		~SpringAudioApi_NativeWindows() = default;
	private:
	};
}