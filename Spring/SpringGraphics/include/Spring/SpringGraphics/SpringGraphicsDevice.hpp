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
		std::vector<GraphicsSurface*> surfaces = {};
	};


	class GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsDeviceDesc& desc, SpringGraphicsApi* api) : m_desc(desc) {};
		virtual ~GraphicsDevice() = default;

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain) = 0;
		virtual bool createGraphicsPipeline(GraphicsPipelineDesc& desc, GaphicsPipeline* swapchain) = 0;
	protected:
		GraphicsDeviceDesc m_desc;
	};
}