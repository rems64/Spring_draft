#pragma once

#include <Spring/SpringGraphics/SpringGraphicsResources.hpp>

namespace spring::graphics
{
	class SpringWindow;
	class Device
	{
	public:
		virtual ~Device() = default;

		virtual int createSwapChain(SwapChain* swapchain, SwapChainDesc& swapchainDesc, spring::graphics::SpringWindow* window) = 0;
		virtual int createShader(Shader* shader, ShaderStages shaderStage, const void* source, size_t size) = 0;
		virtual int createPipelineState(PipelineState* pipelineState, PipelineStateDesc* desc) = 0;
		virtual int createRenderPass(RenderPass* renderPass, RenderPassDesc* desc) = 0;
	};
}