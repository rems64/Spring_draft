#pragma once
#ifdef SPRING_BUILD_VK

#include "SpringGraphicsDevice.hpp"

//#ifdef _WIN32
//#define VK_USE_PLATFORM_WIN32_KHR
//#endif
//#include <vulkan/vulkan.h>

namespace spring::graphics
{
	struct AllocationCollector;
	struct CommandList_Vk;
	class Device_Vulkan : public Device
	{
	public:
		Device_Vulkan();
		virtual ~Device_Vulkan() override;

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
		vk::Instance m_instance = VK_NULL_HANDLE;
		vk::DebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
		vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		vk::Device m_device = VK_NULL_HANDLE;
		vk::Queue m_graphicsQueue = VK_NULL_HANDLE;
		uint32_t m_graphicsQueueFamilyIdx;

		std::vector<std::shared_ptr<CommandList_Vk>> m_commandLists;
		std::shared_ptr<AllocationCollector> m_allocationCollector;
	};
}

#endif