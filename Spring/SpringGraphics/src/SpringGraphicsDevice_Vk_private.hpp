#pragma once

namespace spring::graphics
{
	struct AllocationCollector
	{
		std::mutex locker;
		VkDevice device;
		VkInstance instance;
		std::deque<VkSwapchainKHR> pending_swapchains = {};
		std::deque<VkSurfaceKHR> pending_surfaces = {};
		std::deque<VkImageView> pending_imageViews = {};
		std::deque<VkShaderModule> pending_shaderModules = {};
		std::deque<VkPipelineLayout> pending_pipelineLayouts = {};
		std::deque<VkRenderPass> pending_renderPasses = {};
		std::deque<VkFramebuffer> pending_framebuffers = {};
		std::deque<VkPipeline> pending_pipelines = {};
		std::deque<VkSemaphore> pending_semaphores = {};
		
		void collect()
		{
			std::lock_guard<std::mutex> guardian(locker);
			while (!pending_semaphores.empty())
			{
				VkSemaphore pending = pending_semaphores.front();
				pending_semaphores.pop_front();
				vkDestroySemaphore(device, pending, nullptr);
			}

			while (!pending_imageViews.empty())
			{
				VkImageView pending = pending_imageViews.front();
				pending_imageViews.pop_front();
				vkDestroyImageView(device, pending, nullptr);
			}

			while (!pending_swapchains.empty())
			{
				VkSwapchainKHR pending = pending_swapchains.front();
				pending_swapchains.pop_front();
				vkDestroySwapchainKHR(device, pending, nullptr);
			}

			while (!pending_surfaces.empty())
			{
				VkSurfaceKHR pending = pending_surfaces.front();
				pending_surfaces.pop_front();
				vkDestroySurfaceKHR(instance, pending, nullptr);
			}

			while (!pending_pipelineLayouts.empty())
			{
				VkPipelineLayout pending = pending_pipelineLayouts.front();
				pending_pipelineLayouts.pop_front();
				vkDestroyPipelineLayout(device, pending, nullptr);
			}

			while (!pending_renderPasses.empty())
			{
				VkRenderPass pending = pending_renderPasses.front();
				pending_renderPasses.pop_front();
				vkDestroyRenderPass(device, pending, nullptr);
			}

			while (!pending_framebuffers.empty())
			{
				VkFramebuffer pending = pending_framebuffers.front();
				pending_framebuffers.pop_front();
				vkDestroyFramebuffer(device, pending, nullptr);
			}

			while (!pending_pipelines.empty())
			{
				VkPipeline pending = pending_pipelines.front();
				pending_pipelines.pop_front();
				vkDestroyPipeline(device, pending, nullptr);
			}

			while (!pending_shaderModules.empty())
			{
				VkShaderModule pending = pending_shaderModules.front();
				pending_shaderModules.pop_front();
				vkDestroyShaderModule(device, pending, nullptr);
			}
		}
	};

	struct Shader_Vk
	{
		std::shared_ptr<AllocationCollector> allocationCollector;
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo stageInfo = {};

		~Shader_Vk()
		{
			std::lock_guard<std::mutex> guardian(allocationCollector->locker);
			allocationCollector->pending_shaderModules.push_back(shaderModule);
		}
	};

	struct SwapChain_Vk
	{
		std::shared_ptr<AllocationCollector> allocationCollector;
		VkSwapchainKHR swapchain;
		VkSurfaceKHR surface;
		
		Texture defaultTexture;
		RenderPass renderPass;

		VkExtent2D extent;
		VkFormat imageFormat;

		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		uint32_t swapchainImageIndex;

		std::vector<VkFramebuffer> framebuffers;

		~SwapChain_Vk()
		{
			std::lock_guard<std::mutex> guard(allocationCollector->locker);
			allocationCollector->pending_surfaces.push_back(surface);
			allocationCollector->pending_swapchains.push_back(swapchain);
			allocationCollector->pending_semaphores.push_back(imageAvailableSemaphore);
			allocationCollector->pending_semaphores.push_back(renderFinishedSemaphore);
			for (uint32_t i = 0; i < swapChainImages.size(); i++)
			{
				allocationCollector->pending_imageViews.push_back(swapChainImageViews[i]);
				allocationCollector->pending_framebuffers.push_back(framebuffers[i]);
			}
		}
	};

	struct PipelineState_Vk
	{
		std::shared_ptr<AllocationCollector> allocationCollector;
		VkPipelineLayout pipelineLayout;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		VkPipelineColorBlendStateCreateInfo colorBlending;
		VkPipelineMultisampleStateCreateInfo multisampling;

		VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		VkPipeline pipeline;

		VkPipelineShaderStageCreateInfo shaderStages[ShaderStagesCount];

		~PipelineState_Vk()
		{
			std::lock_guard<std::mutex> guardian(allocationCollector->locker);
			allocationCollector->pending_pipelineLayouts.push_back(pipelineLayout);
			allocationCollector->pending_pipelines.push_back(pipeline);
		}
	};

	struct RenderPass_Vk
	{
		std::shared_ptr<AllocationCollector> allocationCollector;

		VkRenderPass renderPass;
		VkFramebuffer framebuffer;

		VkRenderPassBeginInfo renderPassBeginInfo;

		~RenderPass_Vk()
		{
			std::lock_guard<std::mutex> guardian(allocationCollector->locker);
			allocationCollector->pending_framebuffers.push_back(framebuffer);
			allocationCollector->pending_renderPasses.push_back(renderPass);
		}
	};

	struct CommandList_Vk
	{
		std::shared_ptr<AllocationCollector> allocationCollector;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		const RenderPass* renderpass;
		const PipelineState* pipelineState;
		bool invalidPipeline = true;
	};

}