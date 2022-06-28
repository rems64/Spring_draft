#pragma once
#include <Spring/SpringGraphics/SpringGraphicsDevice.hpp>

namespace spring::graphics
{
	// Forward declaration
	class SpringWindow;
	class SpringGraphicsApi_Vulkan;

	class GraphicsDevice_Vulkan : public GraphicsDevice
	{
	public:
		GraphicsDevice_Vulkan(GraphicsDeviceDesc& desc, SpringGraphicsApi* api);
		//~GraphicsDevice_Vulkan();

		virtual bool createSwapChain(SwapChainDesc& desc, SwapChain* swapchain, SpringWindow* window) const override;

		bool pickPhysicalDevice();

	private:
		SpringGraphicsApi_Vulkan* m_api;
		VkInstance m_instance;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
	};
}