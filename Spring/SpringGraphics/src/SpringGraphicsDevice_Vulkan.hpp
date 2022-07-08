#pragma once
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

namespace spring::graphics
{
	// Forward declaration
	class SpringWindow;
	class SpringGraphicsApi_Vulkan;
	struct GraphicsSurface_Vulkan;
	struct SwapChainSupportDetails;

	struct AllocationHandler
	{
		std::mutex updateLock;
		VkInstance instance = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;

		uint64_t framecount=0;

		std::deque<std::pair<VkShaderModule, uint64_t>> shaderModules;
		std::deque<std::pair<VkSurfaceKHR, uint64_t>> surfaces;
		std::deque<std::pair<VkSwapchainKHR, uint64_t>> swapchains;
		std::deque < std::pair<VkImageView, uint64_t>> imageviews;
		std::deque<std::pair<VkRenderPass, uint64_t>> renderpasses;
		std::deque<std::pair<VkFramebuffer, uint64_t>> framebuffers;

		~AllocationHandler()
		{
			update(std::numeric_limits<uint64_t>::max(), 0);
		};


		void update(const uint64_t frame, const uint16_t buffer) // Frame is the current frame (or the frame when the deleting starts), buffer is a number of frames to keep alive from frame
		{
			std::lock_guard<std::mutex> guard(updateLock);

			while (!shaderModules.empty())
			{
				if (shaderModules.front().second + buffer >= frame)
					break;
				VkShaderModule shaderModule = shaderModules.front().first;
				shaderModules.pop_front();
				vkDestroyShaderModule(device, shaderModule, nullptr);
			}

			while (!imageviews.empty())
			{
				if (imageviews.front().second + buffer >= frame)
					break;
				VkImageView imageview = imageviews.front().first;
				imageviews.pop_front();
				vkDestroyImageView(device, imageview, nullptr);
			}

			while (!framebuffers.empty())
			{
				if (framebuffers.front().second + buffer >= frame)
					break;
				VkFramebuffer framebuffer = framebuffers.front().first;
				framebuffers.pop_front();
				vkDestroyFramebuffer(device, framebuffer, nullptr);
			}

			while (!renderpasses.empty())
			{
				if (renderpasses.front().second + buffer >= frame)
					break;
				VkRenderPass renderpasse = renderpasses.front().first;
				renderpasses.pop_front();
				vkDestroyRenderPass(device, renderpasse, nullptr);
			}
			
			while (!swapchains.empty())
			{
				if (swapchains.front().second + buffer >= frame) // Inserted in order, therefore all surfaces past this one can be discarded for now
					break;
				VkSwapchainKHR swapchain = swapchains.front().first;
				swapchains.pop_front();
				vkDestroySwapchainKHR(device, swapchain, nullptr);
			}

			while (!surfaces.empty())
			{
				if (surfaces.front().second + buffer >= frame) // Inserted in order, therefore all surfaces past this one can be discarded for now
					break;
				VkSurfaceKHR surface = surfaces.front().first;
				surfaces.pop_front();
				vkDestroySurfaceKHR(instance, surface, nullptr);
			}
		}
	};

	struct FrameResource
	{
		VkFence fences[QueueTypes::Count] = {};

		VkCommandPool initCommandPool = VK_NULL_HANDLE;
		VkCommandBuffer initCommandBuffer = VK_NULL_HANDLE;
	};

	class GraphicsDevice_Vulkan : public GraphicsDevice
	{
	public:
		GraphicsDevice_Vulkan(GraphicsDeviceDesc& desc, SpringGraphicsApi* api);
		virtual ~GraphicsDevice_Vulkan();

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain) override;
		virtual bool createGraphicsPipeline(GraphicsPipelineDesc& desc, GaphicsPipeline* swapchain) override;
		virtual bool createShader(ShaderDesc& desc, Shader* shader) override;

		bool pickPhysicalDevice();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool createDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, GraphicsSurface_Vulkan* surface);
		AllocationHandler* getAllocationHandler() { return m_allocationHandler.get(); };

	private:
		static const uint32_t framesInFlight = 2;
		SpringGraphicsApi_Vulkan* m_api;
		VkInstance m_instance;

		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		uint32_t graphicsQueueFamily;
		uint32_t computeQueueFamily;
		uint32_t transfertQueueFamily;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		FrameResource m_frames[framesInFlight];

		GraphicsDeviceDesc m_desc;
		std::vector<const char*> m_deviceRequiredExtensions;
		
		Ref<AllocationHandler> m_allocationHandler;
	};
}