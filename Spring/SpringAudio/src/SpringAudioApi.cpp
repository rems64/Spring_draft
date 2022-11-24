#include <Spring/SpringAudio/ISpringAudioApi.hpp>

#if defined(SP_WIN32)
#include "SpringAudioApi_NativeWindows.hpp"

#elif defined(SP_LINUX)

#endif
namespace spring::audio
{
	Scope<ISpringAudioApi> ISpringAudioApi::build()
	{
#if defined(SP_WIN32)
		return makeScope<SpringAudioApi_NativeWindows>();
#elif defined(SP_LINUX)
#endif
	}
}