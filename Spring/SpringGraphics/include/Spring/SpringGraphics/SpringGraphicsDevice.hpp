#pragma once

#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>

namespace spring::graphics
{
	class SpringGraphicsApi;
	class SpringWindow;

	struct GraphicsDeviceDesc
	{
		bool onlyDiscrete = false;
		bool supportPresent = true;
	};


	class GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsDeviceDesc& desc, SpringGraphicsApi* api) : m_desc(desc) {};
		virtual ~GraphicsDevice() = default;

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain, SpringWindow* window) const = 0;
	protected:
		GraphicsDeviceDesc m_desc;
	};
}