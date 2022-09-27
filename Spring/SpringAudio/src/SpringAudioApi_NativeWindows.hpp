#pragma once

#include <mmdeviceapi.h>
#include <Spring/SpringAudio/ISpringAudioApi.hpp>

namespace spring::audio
{
	class Device;
	class SpringAudioApi_NativeWindows final : public ISpringAudioApi
	{
	public:
		SpringAudioApi_NativeWindows();
		~SpringAudioApi_NativeWindows() = default;

		Scope<audio::Device> createDevice(DeviceDesc& desc) override;

	private:
		spNativeLib m_mmDeviceApi;
		IMMDeviceEnumerator* m_deviceEnumerator;
	};
}