#include "SpringAudioApi_NativeWindows.hpp"

#include <Spring/SpringAudio/SpringAudioDevice.hpp>

#include <mmdeviceapi.h>
#include <Audioclient.h>
//#include <audiopolicy.h>
#include <combaseapi.h>

constexpr CLSID gClsid_MmDeviceEnumerator = __uuidof(MMDeviceEnumerator);
constexpr IID gIid_ImmDeviceEnumerator    = __uuidof(IMMDeviceEnumerator);
constexpr IID gIid_IAudioClient           = __uuidof(IAudioClient);
constexpr IID gIid_IAudioRenderClient     = __uuidof(IAudioRenderClient);

namespace spring::audio
{
	SpringAudioApi_NativeWindows::SpringAudioApi_NativeWindows() : m_deviceEnumerator(nullptr)
	{
        m_mmDeviceApi = core::loadLibrary("Mmdevapi.dll");
		if (!m_mmDeviceApi)
			SPRING_ERROR("Failed to load Mmdevapi.dll")

        HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
        if (!SUCCEEDED(hr))
	        SPRING_ERROR("Failed to initialize COM for Spring Audio")

        hr = CoCreateInstance(gClsid_MmDeviceEnumerator, nullptr, CLSCTX_ALL, gIid_ImmDeviceEnumerator, reinterpret_cast<void**>(&m_deviceEnumerator));
		spdlog::trace("{0:x}", hr);

		spdlog::info("Successfully initialized native windows audio!");
	}

	Scope<audio::Device> SpringAudioApi_NativeWindows::createDevice(DeviceDesc& desc)
	{
        IMMDevice* device = nullptr;
        HRESULT hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
		if(!SUCCEEDED(hr))
			SPRING_ERROR("Failde to get default audio endpoint")
        return makeScope<audio::Device>();
	}
}