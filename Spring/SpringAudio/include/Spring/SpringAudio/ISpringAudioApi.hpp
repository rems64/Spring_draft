#pragma once

#include <Spring/SpringCore/SpringCore.hpp>

namespace spring::audio
{
	class Device;
	enum class DeviceNature
	{
		Input,
		Output
	};
	struct DeviceDesc
	{
		DeviceNature nature=DeviceNature::Output;
	};

	class ISpringAudioApi
	{
	public:
		virtual Scope<audio::Device> createDevice(DeviceDesc& desc) = 0;

		static Scope<ISpringAudioApi> build();
	};

}