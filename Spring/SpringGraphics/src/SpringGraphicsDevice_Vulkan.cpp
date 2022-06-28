#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>

#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#ifdef SE_WINDOWS
#include <GLFW/glfw3.h>
#endif

namespace spring::graphics
{
	struct SwapChain_Vulkan
	{
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;

		bool hasSurface = false;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	GraphicsDevice_Vulkan::GraphicsDevice_Vulkan(GraphicsDeviceDesc& desc, SpringGraphicsApi* api) : GraphicsDevice(desc, api)
	{
		m_api = static_cast<SpringGraphicsApi_Vulkan*>(api);
		m_instance = *m_api->getInstance();
		pickPhysicalDevice();
	}

	GraphicsDevice_Vulkan::~GraphicsDevice_Vulkan()
	{

	}

	bool GraphicsDevice_Vulkan::pickPhysicalDevice()
	{
		uint32_t physicalDevicesCount;
		vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
		vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevices.data());
		spdlog::info("{} physical devices available:", physicalDevicesCount);
		for (VkPhysicalDevice& pDevice : physicalDevices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(pDevice, &properties);
			if (properties.deviceType != VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				continue;

			uint32_t extCount;
			vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extCount, nullptr);
			std::vector<VkExtensionProperties> exts(extCount);
			vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extCount, exts.data());
			//bool supportExt = checkDeviceExtensionSupport(pDevice);
			//bool swapChainAdequate = false;
			//if (supportExt) {
				//SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pDevice);
				//swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			//}
			//if (!supportExt || !swapChainAdequate)
				//continue;
			spdlog::info("  Device ({}) --> {} available extensions", properties.deviceName, extCount);
			m_physicalDevice = pDevice;
			//msaaSamples = getMaxUsableSampleCount();
			return true;
		}
		return false;
	}

	bool GraphicsDevice_Vulkan::createSwapChain(SwapChainDesc& desc, SwapChain* swapchain, SpringWindow* window) const
	{
		auto internal_state = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);
		if (!swapchain->isValid())
		{
			internal_state = makeRef<SwapChain_Vulkan>();
		}
		swapchain->desc = desc;
		swapchain->internal_state = internal_state;

		if (desc.hasSurface && internal_state->surface == VK_NULL_HANDLE)
		{
			if (!window)
			{
				spdlog::error("Invalid window in swapchain creation!");
				throw std::runtime_error("Invalid window");
			}
			internal_state->hasSurface = true;

#ifdef SE_WINDOWS
			if (glfwCreateWindowSurface(*m_api->getInstance(), window->getHandle(), nullptr, &internal_state->surface) != VK_SUCCESS)
			{
				spdlog::error("Failed to create window surface from glfw");
			}
#elif GLFW3
#else
			spring::core::error("Can't create a window, no supported backend");
			return nullptr;
#endif
		}

		// Swapchain support
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, internal_state->surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, internal_state->surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, internal_state->surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, internal_state->surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, internal_state->surface, &presentModeCount, details.presentModes.data());
		}

		return false;
	}
}