#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

//#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"

namespace spring::graphics
{
	Scope<SpringGraphicsApi> SpringGraphicsApi::build()
	{
		SP_PROFILE_FUNCTION();
#ifdef SPRING_BUILD_VK
		return makeScope<SpringGraphicsApi_Vulkan>();
#elif SPRING_BUILD_GL
#endif
	}
}