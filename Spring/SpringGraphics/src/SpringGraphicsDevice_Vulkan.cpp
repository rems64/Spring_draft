#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>

#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>
#include <Spring/SpringGraphics/ISpringWindow.hpp>
#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include <Spring/SpringGraphics/SpringGraphicsVulkanUtils.hpp>
#ifdef SE_WINDOWS
#include <GLFW/glfw3.h>
#endif

namespace spring::graphics
{
	struct SwapChain_Vulkan
	{
		Ref<AllocationHandler> allocationHandler;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;

		bool hasSurface = false;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<VkImage> swapchainImages = {};
		std::vector<VkImageView> swapchainImageViews = {};
		std::vector<VkFramebuffer> swapchainFramebuffers = {};
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		~SwapChain_Vulkan()
		{
			allocationHandler->swapchains.push_back(std::make_pair(swapchain, 0));
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	GraphicsDevice_Vulkan::GraphicsDevice_Vulkan(GraphicsDeviceDesc& desc, SpringGraphicsApi* api) : GraphicsDevice(desc, api), m_deviceRequiredExtensions{}, m_desc(desc), m_allocationHandler(makeRef<AllocationHandler>())
	{
		m_api = static_cast<SpringGraphicsApi_Vulkan*>(api);
		m_instance = *m_api->getInstance();
		m_allocationHandler->instance = m_instance;

		if (m_desc.supportPresent)
		{
			if (m_desc.surfaces.size()<=0)
			{
				spdlog::error("No surface provided to device while it should support presentation!");
				throw std::runtime_error("Invalid surface");
			}
			m_deviceRequiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
		if (!pickPhysicalDevice())
		{
			spdlog::error("Unable to find suitable GPU...");
		}
		createDevice();
		m_allocationHandler->device = m_device;
	}

	GraphicsDevice_Vulkan::~GraphicsDevice_Vulkan()
	{
		m_allocationHandler->update(std::numeric_limits<uint64_t>::max(), 0);

		vkDestroyDevice(m_device, nullptr);
	}

	bool GraphicsDevice_Vulkan::pickPhysicalDevice()
	{
		uint32_t physicalDevicesCount;
		vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
		vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevices.data());
		spdlog::info("There {} {} physical device{} available:", physicalDevicesCount>1?"are":"is", physicalDevicesCount, physicalDevicesCount>1?"s":"");
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
			bool supportExt = checkDeviceExtensionSupport(pDevice);
			bool swapChainAdequate = true;
			if (!supportExt)
				continue;
			std::vector<SwapChainSupportDetails> swapChainSupports{};
			for (auto& surface : m_desc.surfaces)
			{
				Ref<GraphicsSurface_Vulkan> surface_vk = std::static_pointer_cast<GraphicsSurface_Vulkan>(surface->internal_state);
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pDevice, surface_vk.get());
				swapChainAdequate &= !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			}
			if (!supportExt || !swapChainAdequate)
				continue;
			spdlog::info("  Device \"{}\" ({}) --> {} available extensions", properties.deviceName, getPhysicalDeviceTypeName(properties.deviceType), extCount);
			m_physicalDevice = pDevice;
			//msaaSamples = getMaxUsableSampleCount();
			return true;
		}
		return false;
	}

	bool GraphicsDevice_Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceRequiredExtensions.begin(), m_deviceRequiredExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails GraphicsDevice_Vulkan::querySwapChainSupport(VkPhysicalDevice device, GraphicsSurface_Vulkan* surface) {
		SwapChainSupportDetails details = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface->surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}

	QueueFamilyIndices GraphicsDevice_Vulkan::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (m_desc.supportPresent)
			{
				if (m_desc.surfaces.size() <= 0)
					spdlog::error("Shouldn't be here...");
				VkBool32 presentSupport = false;
				VkSurfaceKHR surface = std::static_pointer_cast<GraphicsSurface_Vulkan>(m_desc.surfaces[0]->internal_state)->surface;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport) {
					indices.presentFamily = i;
				}
			}

			i++;
		}

		return indices;
	}

	bool GraphicsDevice_Vulkan::createDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures
		{
			.samplerAnisotropy = VK_TRUE
		};

		VkDeviceCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = (uint32_t)m_deviceRequiredExtensions.size(),
			.ppEnabledExtensionNames = m_deviceRequiredExtensions.data(),
			.pEnabledFeatures = &deviceFeatures,
		};

#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
		// since Vulkan 1.3 this is useless, kept for compatibility :)
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_api->m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_api->m_validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif
		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
		return true;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
		//return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}


	bool GraphicsDevice_Vulkan::createSwapChain(SwapChainDesc& desc, SwapChain* swapchain)
	{
		auto internal_state = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);
		if (!swapchain->isValid())
		{
			internal_state = makeRef<SwapChain_Vulkan>();
		}
		internal_state->allocationHandler = m_allocationHandler;
		swapchain->desc = desc;
		swapchain->internal_state = internal_state;

		if (desc.hasSurface && internal_state->surface == VK_NULL_HANDLE)
		{
			internal_state->hasSurface = true;
			internal_state->surface = std::static_pointer_cast<GraphicsSurface_Vulkan>(desc.surface->internal_state)->surface;
		}

		// Swapchain support
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, std::static_pointer_cast<GraphicsSurface_Vulkan>(desc.surface->internal_state).get());

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = {};

		if (swapChainSupport.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = swapChainSupport.capabilities.currentExtent;
		}
		else {
			extent = {
				static_cast<uint32_t>(desc.surface->width),
				static_cast<uint32_t>(desc.surface->height)
			};

			extent.width = std::clamp(extent.width, swapChainSupport.capabilities.minImageExtent.width, swapChainSupport.capabilities.maxImageExtent.width);
			extent.height = std::clamp(extent.height, swapChainSupport.capabilities.minImageExtent.height, swapChainSupport.capabilities.maxImageExtent.height);
		}

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		desc.hasSurface;
		VkSwapchainCreateInfoKHR createInfo
		{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = internal_state->surface,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		};

		QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &internal_state->swapchain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_device, internal_state->swapchain, &imageCount, nullptr);
		internal_state->swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, internal_state->swapchain, &imageCount, internal_state->swapchainImages.data());
		internal_state->swapChainImageFormat = surfaceFormat.format;
		internal_state->swapChainExtent = extent;

		return true;
	}
}