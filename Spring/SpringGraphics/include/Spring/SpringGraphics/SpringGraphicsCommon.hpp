#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::graphics
{
	// Structure from Wicked engine : https://github.com/turanszkij/WickedEngine
	struct GraphicsDeviceChild
	{
		Ref<void> internal_state;
		inline bool isValid() { return internal_state.get() != nullptr; };
	};


	struct SwapChainDesc
	{
		uint32_t width = 640;
		uint32_t height = 480;
		bool hasSurface = true;
	};

	struct SwapChain : public GraphicsDeviceChild
	{
		SwapChainDesc desc;

		inline const SwapChainDesc& getDesc() const { return desc; };
	};
}