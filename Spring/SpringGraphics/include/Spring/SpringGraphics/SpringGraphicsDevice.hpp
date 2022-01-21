#pragma once

#include <Spring/SpringGraphics/SpringGraphicsResources.hpp>

namespace spring::graphics
{
	class SpringWindow;
	class Device
	{
	public:
		virtual ~Device() = default;

		virtual int createSwapChain(SwapChain* swapchain, SwapChainDesc swapchainDesc, spring::graphics::SpringWindow* window) = 0;
	};
}