#include <Spring/SpringAudio/ISpringAudioApi.hpp>

#include "SpringAudioApi_NativeWindows.hpp"

namespace spring::audio
{
	Scope<ISpringAudioApi> ISpringAudioApi::build()
	{
#ifdef SP_WIN32
		return makeScope<SpringAudioApi_NativeWindows>();
#endif
	}
}