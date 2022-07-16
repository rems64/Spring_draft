#include "SpringAudioApi_NativeWindows.hpp"

#include <mmdeviceapi.h>
//#include <Audioclient.h>
//#include <audiopolicy.h>
#include <combaseapi.h>

namespace spring::audio
{
	SpringAudioApi_NativeWindows::SpringAudioApi_NativeWindows()
	{
		const spNativeLib mmdeviceApi = core::loadLibrary("Mmdevapi.dll");
		if (!mmdeviceApi)
			SPRING_ERROR("Failed to load Mmdevapi.dll");

		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		IMMDeviceEnumerator* pEnumerator = NULL;
		HRESULT res = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&pEnumerator));
		spdlog::trace("{0:x}", res);

		spdlog::info("Successfully initialized native windows audio!");
	}
}