#pragma once

#include <mmdeviceapi.h>
#include <Spring/SpringCore/SpringPlatform.hpp>
#include <Spring/SpringAudio/ISpringAudioApi.hpp>

namespace spring::audio
{
	class Device;
	class SpringAudioApi_NativeWindows final : public ISpringAudioApi
	{
	public:
		SpringAudioApi_NativeWindows();

		Scope<audio::Device> createDevice(DeviceDesc& desc) override;

	private:
		spNativeLib m_mmDeviceApi;
		IMMDeviceEnumerator* m_deviceEnumerator;
	};
}