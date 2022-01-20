#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringGraphics/SpringGraphics.hpp>

#include <cstdio>
#include <memory>
#include <iostream>

int main()
{
	std::unique_ptr<spring::graphics::Device_Vulkan> device = std::make_unique<spring::graphics::Device_Vulkan>();
	printf("%p\n", device.get());
	return 0;
}