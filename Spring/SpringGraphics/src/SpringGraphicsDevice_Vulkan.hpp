#pragma once
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

#include "vk_mem_alloc.h"

namespace spring::graphics
{
	// Forward declaration
	class SpringWindow;
	class SpringGraphicsApi_Vulkan;
    class GraphicsDevice_Vulkan;
	struct GraphicsSurface_Vulkan;
	struct SwapChainSupportDetails;

	struct AllocationHandler
	{
        VmaAllocator allocator = VK_NULL_HANDLE;
        std::mutex updateLock;
		VkInstance instance = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;

		uint64_t framecount=0;

        std::deque<std::pair<std::pair<VkBuffer, VmaAllocation>, uint64_t>> buffers;
		std::deque<std::pair<VkShaderModule, uint64_t>> shaderModules;
		std::deque<std::pair<VkPipelineLayout, uint64_t>> pipelineLayouts;
        std::deque<std::pair<VkPipeline, uint64_t>> pipelines;
		std::deque<std::pair<VkSurfaceKHR, uint64_t>> surfaces;
		std::deque<std::pair<VkSwapchainKHR, uint64_t>> swapchains;
		std::deque<std::pair<VkImageView, uint64_t>> imageviews;
		std::deque<std::pair<VkRenderPass, uint64_t>> renderpasses;
		std::deque<std::pair<VkFramebuffer, uint64_t>> framebuffers;

		~AllocationHandler()
		{
			update(std::numeric_limits<uint64_t>::max(), 0);
            vmaDestroyAllocator(allocator);
            vkDestroyDevice(device, nullptr);
        }


		void update(const uint64_t frame, const uint16_t buffer) // Frame is the current frame (or the frame when the deleting starts), buffer is a number of frames to keep alive from frame
		{
			std::lock_guard<std::mutex> guard(updateLock);

            while (!buffers.empty())
            {
                if (buffers.front().second + buffer >= frame)
                    break;
                std::pair<VkBuffer, VmaAllocation> buff = buffers.front().first;
                buffers.pop_front();
                vmaDestroyBuffer(allocator, buff.first, buff.second);
            }

			while (!shaderModules.empty())
			{
				if (shaderModules.front().second + buffer >= frame)
					break;
				VkShaderModule shaderModule = shaderModules.front().first;
				shaderModules.pop_front();
				vkDestroyShaderModule(device, shaderModule, nullptr);
			}

            while (!pipelineLayouts.empty()) {
                if (pipelineLayouts.front().second + buffer >= frame)
                    break;
                VkPipelineLayout pipelineLayout = pipelineLayouts.front().first;
                pipelineLayouts.pop_front();
                vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            }

            while (!pipelines.empty()) {
                if (pipelines.front().second + buffer >= frame)
                    break;
                VkPipeline pipeline = pipelines.front().first;
                pipelines.pop_front();
                vkDestroyPipeline(device, pipeline, nullptr);
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

    struct CopyHandler {
        struct CopyTask {
            Buffer buffer;
            VkCommandBuffer cmd;
        };
        GraphicsDevice_Vulkan* device = nullptr;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        uint64_t count = 0;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<CopyTask> freeTasks = {};
        std::vector<CopyTask> ongoing = {};
        std::vector<VkCommandBuffer> toSubmit = {};

        void init(GraphicsDevice_Vulkan* device);
        CopyTask allocate(size_t size);
        void submit(const CopyTask& task);
        void performCopy();
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
		GraphicsDevice_Vulkan(GraphicsDeviceDesc desc, SpringGraphicsApi* api);
		~GraphicsDevice_Vulkan() override;

		bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain) override;
        bool createRenderPass(RenderPassDesc& desc, RenderPass* renderpass) override;
		bool createGraphicsPipeline(GraphicsPipelineDesc& desc, GraphicsPipeline* swapchain) override;
		bool validatePipeline(const Ref<CommandList>& cmd) override;
        bool createBuffer(BufferDesc desc, Buffer* buffer, void* data=nullptr) override;
        bool createTexture(TextureDesc desc, Texture* texture) override;
        bool createShader(ShaderDesc desc, Shader* shader) override;
        bool createCommandBuffer(const Ref<CommandBuffer>& commandBuffer) override;

        bool bindPipeline(const Ref<CommandList>& cmd, const Ref<GraphicsPipeline>& pipeline) override;
        bool bindVertexBuffer(const Ref<CommandList>& cmd, const Ref<Buffer>& buffer, uint32_t first) override;
        bool beginCommandList(const Ref<CommandList>& cmd) override;
        bool beginRenderPass(const Ref<CommandList>& cmd, const Ref<SwapChain>& swapChain) override;
        bool endRenderPass(const Ref<CommandList>& cmd) override;
        bool submitCommandList(const Ref<CommandList>& cmd) override;

        bool fastDraw(const Ref<SwapChain>& swapchain, const Ref<GraphicsPipeline>& pipeline, const Ref<CommandList>& cmd) override;

		bool pickPhysicalDevice();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
		bool createDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);
		[[nodiscard]] AllocationHandler* getAllocationHandler() const;
    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        friend CopyHandler;

    private:
		static constexpr uint32_t framesInFlight = 2;
		SpringGraphicsApi_Vulkan* m_api;
		const VkInstance* m_instance;

		VkPhysicalDevice m_physicalDevice{};
		VkDevice m_device;
		QueueFamilyIndices m_queueFamilies{};
		VkQueue m_graphicsQueue{};
		VkQueue m_presentQueue{};
		FrameResource m_frames[framesInFlight];

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
        VkPhysicalDeviceVulkan11Features physicalDeviceFeatures11 = {};
        VkPhysicalDeviceVulkan12Features physicalDeviceFeatures12 = {};

        std::vector<const char*> m_deviceRequiredExtensions;

        std::unordered_map<size_t, VkPipeline> m_pipelines;

        Ref<CopyHandler> m_copyHandler;
		Ref<AllocationHandler> m_allocationHandler;

        struct CommandQueue
        {
            VkQueue queue = VK_NULL_HANDLE;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            std::vector<SwapChain> swapchain_updates;
            std::vector<VkSwapchainKHR> submit_swapchains;
            std::vector<uint32_t> submit_swapChainImageIndices;
            std::vector<VkPipelineStageFlags> submit_waitStages;
            std::vector<VkSemaphore> submit_waitSemaphores;
            std::vector<uint64_t> submit_waitValues;
            std::vector<VkSemaphore> submit_signalSemaphores;
            std::vector<uint64_t> submit_signalValues;
            std::vector<VkCommandBuffer> submit_cmds;

            bool sparse_binding_supported = false;
            std::mutex sparse_mutex;

            void submit(GraphicsDevice_Vulkan* device, VkFence fence);

        } queues[(size_t)QueueTypes::Count];
	};
}