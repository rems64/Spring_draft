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

		std::deque<std::pair<VkSurfaceKHR, uint64_t>> surfaces;
		std::deque<std::pair<VkSwapchainKHR, uint64_t>> swapchains;

		~AllocationHandler()
		{
			update(std::numeric_limits<uint64_t>::max(), 0);
		};


		void update(const uint64_t frame, const uint16_t buffer) // Frame is the current frame (or the frame when the deleting starts), buffer is a number of frames to keep alive from frame
		{
			std::lock_guard<std::mutex> guard(updateLock);

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

	class GraphicsDevice_Vulkan : public GraphicsDevice
	{
	public:
		GraphicsDevice_Vulkan(GraphicsDeviceDesc& desc, SpringGraphicsApi* api);
		virtual ~GraphicsDevice_Vulkan();

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain) override;

		bool createSurface(VkSurfaceKHR* surface);
		bool pickPhysicalDevice();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool createDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, GraphicsSurface_Vulkan* surface);
		AllocationHandler* getAllocationHandler() { return m_allocationHandler.get(); };

	private:
		SpringGraphicsApi_Vulkan* m_api;
		VkInstance m_instance;

		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		uint32_t graphicsQueueFamily;
		uint32_t computeQueueFamily;
		uint32_t transfertQueueFamily;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		GraphicsDeviceDesc m_desc;
		std::vector<const char*> m_deviceRequiredExtensions;
		
		Ref<AllocationHandler> m_allocationHandler;
	};
}