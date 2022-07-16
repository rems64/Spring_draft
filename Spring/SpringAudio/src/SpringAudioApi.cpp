#include <Spring/SpringAudio/ISpringAudioApi.hpp>

#include "SpringAudioApi_NativeWindows.hpp"

namespace spring::audio
{
	Scope<ISpringAudioApi> ISpringAudioApi::build()
	{
#ifdef SP_WINDOWS
		return makeScope<SpringAudioApi_NativeWindows>();
#endif
	}
}