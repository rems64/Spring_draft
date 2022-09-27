#pragma once

#include <Spring/SpringAudio/SpringAudioDevice.hpp>

namespace spring::audio
{
	class Device_NativeWindows : public audio::Device
	{
		Device_NativeWindows();
		~Device_NativeWindows();
	};
}