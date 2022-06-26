#pragma once
#ifdef SPRING_BUILD_GL

#include "SpringGraphicsDevice.hpp"
#include <GL/GL.h>


namespace spring::graphics
{
	struct AllocationCollector;
	struct CommandList_Vk;
	class Device_OpenGL : public Device
	{
	public:
		Device_OpenGL();
		virtual ~Device_OpenGL() override;

		virtual int createSwapChain(SwapChain* swapchain, SwapChainDesc& swapchainDesc, SpringSurface surface) override;
		virtual int createShader(Shader* shader, ShaderStages shaderStage, const void* source, size_t size) override;
		virtual int createPipelineState(PipelineState* pipelineState, PipelineStateDesc* desc) override;
		virtual int createRenderPass(RenderPass* renderPass, RenderPassDesc* desc) override;
		virtual CommandList beginCommandList(SwapChain* swapchain) override;
		virtual int beginRenderPass(SwapChain* swapchain, CommandList* commandList) override;

		virtual int bindPipelineState(const PipelineState* pipelineState, CommandList commandList) override;
		virtual int bindRenderPass(const RenderPass* renderpass, CommandList commandList) override;
		virtual int renderFrame(SwapChain* swapchain, CommandList* commandList) override;

	private:
		int validatePipeline(CommandList* commandList);

	protected:
		HDC m_context = nullptr;
	};
}

#endif