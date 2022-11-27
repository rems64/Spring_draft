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
		std::vector<Ref<SpringWindow>> drawingWindows;
		//std::vector<GraphicsSurface*> surfaces = {};
	};


	class GraphicsDevice
	{
	public:
		GraphicsDevice(GraphicsDeviceDesc desc, SpringGraphicsApi*) : m_desc(std::move(desc)) {}
		virtual ~GraphicsDevice() = default;

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain) = 0;
        virtual bool createRenderPass(RenderPassDesc& desc, RenderPass* renderpass) = 0;
		virtual bool createGraphicsPipeline(GraphicsPipelineDesc& desc, GraphicsPipeline* swapchain) = 0;
		virtual bool validatePipeline(const Ref<CommandList>& cmd) = 0;
        virtual bool createBuffer(BufferDesc desc, Buffer* buffer, void* data) = 0;
        virtual bool createTexture(TextureDesc desc, Texture* texture) = 0;
        virtual bool createShader(ShaderDesc desc, Shader* shader) = 0;
        virtual bool createCommandBuffer(const Ref<CommandBuffer>& commandBuffer) = 0;

        virtual bool bindPipeline(const Ref<CommandList>& cmd, const Ref<GraphicsPipeline>& pipeline) = 0;
        virtual bool bindVertexBuffer(const Ref<CommandList>& cmd, const Ref<Buffer>& buffer, uint32_t first) = 0;
        virtual bool beginCommandList(const Ref<CommandList>& cmd) = 0;
        virtual bool beginRenderPass(const Ref<CommandList>& cmd, const Ref<SwapChain>& swapChain) = 0;
        virtual bool endRenderPass(const Ref<CommandList>& cmd) = 0;
        virtual bool submitCommandList(const Ref<CommandList>& cmd) = 0;
        virtual bool fastDraw(const Ref<SwapChain>& swapchain, const Ref<GraphicsPipeline>& pipeline, const Ref<CommandList>& cmd) = 0;

        static constexpr uint32_t framesInFlight = 2;
	protected:
		GraphicsDeviceDesc m_desc;
	};
}