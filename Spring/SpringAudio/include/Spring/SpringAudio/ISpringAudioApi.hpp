#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::audio
{
	class Device;
	enum class DeviceNature
	{
		Capture,
		Rendering
	};
	struct DeviceDesc
	{
		DeviceNature nature=DeviceNature::Rendering;
	};

	class ISpringAudioApi
	{
	public:
		virtual Scope<audio::Device> createDevice(DeviceDesc& desc) = 0;

		static Scope<ISpringAudioApi> build();
	};

}