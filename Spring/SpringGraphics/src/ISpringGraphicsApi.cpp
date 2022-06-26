#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>
#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>

namespace spring::graphics
{
	SpringGraphicsApi* SpringGraphicsApi::build()
	{
#ifdef SPRING_BUILD_VK
		return new SpringGraphicsApi_Vulkan();
#elif SPRING_BUILD_GL
#endif
	}
}