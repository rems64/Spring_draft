#pragma once

namespace spring::graphics
{
	struct SwapChainDesc
	{
		uint32_t width;
		uint32_t height;
		bool fullscreen;
		bool vsync;
	};

	struct SwapChain
	{
		SwapChainDesc desc;

		constexpr const SwapChainDesc& getDesc() { return desc; };
	};
}