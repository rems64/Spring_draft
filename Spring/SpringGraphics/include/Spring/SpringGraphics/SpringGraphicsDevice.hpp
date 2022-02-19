#pragma once

#include <Spring/SpringGraphics/SpringGraphicsResources.hpp>

namespace spring::graphics
{
	class SpringWindow;
	struct SpringSurface;
	class Device
	{
	public:
		virtual ~Device() = default;

		virtual int createSwapChain(SwapChain* swapchain, SwapChainDesc& swapchainDesc, SpringSurface& surface) = 0;
		virtual int createShader(Shader* shader, ShaderStages shaderStage, const void* source, size_t size) = 0;
		virtual int createPipelineState(PipelineState* pipelineState, PipelineStateDesc* desc) = 0;
		virtual int createRenderPass(RenderPass* renderPass, RenderPassDesc* desc) = 0;
		virtual CommandList beginCommandList(SwapChain* swapchain) = 0;
		virtual int beginRenderPass(SwapChain* swapchain, CommandList* commandList) = 0;
		
		virtual int bindPipelineState(const PipelineState* pipelineState, CommandList commandList) = 0;
		virtual int bindRenderPass(const RenderPass* renderpass, CommandList commandList) = 0;
		virtual int renderFrame(SwapChain* swapchain, CommandList* commandList) = 0;
	};
}