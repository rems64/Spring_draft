//#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>
#include "SpringGraphicsDevice_Vulkan.hpp"

#include <Spring/SpringCore/SpringProfiler.hpp>

#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>
#include <Spring/SpringGraphics/ISpringWindow.hpp>
//#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"
#include <Spring/SpringGraphics/SpringGraphicsVulkanUtils.hpp>


namespace spring::graphics
{
	struct Shader_Vulkan
	{
		Ref<AllocationHandler> allocationHandler;

		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo shaderStageInfo = {};

		~Shader_Vulkan()
		{
			std::lock_guard<std::mutex> locker(allocationHandler->updateLock);

			if (shaderModule != VK_NULL_HANDLE)
				allocationHandler->shaderModules.emplace_back(shaderModule, allocationHandler->framecount);
		}
	};

	struct RenderPass_Vulkan
	{
		Ref<AllocationHandler> allocationHandler;
		VkRenderPass renderpass = VK_NULL_HANDLE;
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkRenderPassBeginInfo beginInfo = {};
		VkClearValue clearColors[9] = {};

		~RenderPass_Vulkan()
		{
			if (allocationHandler == nullptr)
				return;
			std::lock_guard locker(allocationHandler->updateLock);
			uint64_t framecount = allocationHandler->framecount;
			if (renderpass)
				allocationHandler->renderpasses.emplace_back(renderpass, framecount);

			if (framebuffer)
				allocationHandler->framebuffers.emplace_back(framebuffer, framecount);
		}
	};


	struct SwapChain_Vulkan
	{
		Ref<AllocationHandler> allocationHandler;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;

		bool hasSurface = false;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<VkImage> swapchainImages = {};
		std::vector<VkImageView> swapchainImageViews = {};
		std::vector<VkFramebuffer> swapchainFramebuffers = {};
		VkFormat swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
		VkExtent2D swapChainExtent = {};

		RenderPass renderpass = {};

		~SwapChain_Vulkan()
		{
			uint64_t framecount = allocationHandler->framecount;
			for (auto& imageview : swapchainImageViews)
			{
				allocationHandler->imageviews.emplace_back(imageview, framecount);
			}
			for (auto& framebuffer : swapchainFramebuffers)
			{
				allocationHandler->framebuffers.emplace_back(framebuffer, framecount);
			}
			allocationHandler->swapchains.emplace_back(swapchain, framecount);
		}
	};

	struct GraphicsPipeline_Vulkan
	{
		VkViewport viewport = {};
		VkRect2D scissor = {};
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	GraphicsDevice_Vulkan::GraphicsDevice_Vulkan(GraphicsDeviceDesc desc, SpringGraphicsApi* api) :
		GraphicsDevice(std::move(desc), api), m_device(nullptr),
		m_allocationHandler(makeRef<AllocationHandler>())
	{
		SP_PROFILE_FUNCTION();

		m_api = dynamic_cast<SpringGraphicsApi_Vulkan*>(api);
		m_instance = m_api->getInstance();
		m_allocationHandler->instance = *m_instance;

		if (m_desc.supportPresent)
		{
			if (m_desc.drawingWindows.empty())
			{
				spdlog::error("No surface provided to device while it should support presentation!");
				throw std::runtime_error("Invalid surface");
			}
			for (const auto& win : m_desc.drawingWindows)
			{
				win->buildSurface(m_api);
			}
			m_deviceRequiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
		if (!pickPhysicalDevice())
		{
			spdlog::error("Unable to find suitable GPU...");
		}
		createDevice();
		m_allocationHandler->device = m_device;
		for (uint32_t frame = 0; frame < framesInFlight; frame++)
		{
			for (uint32_t queueType = 0; queueType < Count; queueType++)
			{
				VkFenceCreateInfo fenceCreateInfo =
				{
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.flags = VK_FENCE_CREATE_SIGNALED_BIT // For preventing waiting for a fence which never signals
				};
				if (vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_frames[frame].fences[queueType]) !=
					VK_SUCCESS)
				{
					SPRING_ERROR("Failed to create fence!")
				}
			}

			QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

			VkCommandPoolCreateInfo poolInfo
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()

			};

			if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_frames[frame].initCommandPool) != VK_SUCCESS)
			{
				SPRING_ERROR("failed to create command pool!")
			}

			VkCommandBufferAllocateInfo allocInfo
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = m_frames[frame].initCommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1
			};

			if (vkAllocateCommandBuffers(m_device, &allocInfo, &m_frames[frame].initCommandBuffer) != VK_SUCCESS)
				SPRING_ERROR("Failed to allocate command buffer")

			VkCommandBufferBeginInfo beginInfo
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
			};

			if (vkBeginCommandBuffer(m_frames[frame].initCommandBuffer, &beginInfo) != VK_SUCCESS)
				SPRING_ERROR("Failed to begin command buffer!")
		}
	}

	GraphicsDevice_Vulkan::~GraphicsDevice_Vulkan()
	{
		SP_PROFILE_FUNCTION();

		for (auto& frame : m_frames)
		{
			for (int queue = 0; queue < QueueTypes::Count; ++queue)
			{
				vkDestroyFence(m_device, frame.fences[queue], nullptr);
			}
			vkDestroyCommandPool(m_device, frame.initCommandPool, nullptr);
		}

		m_allocationHandler->update(std::numeric_limits<uint64_t>::max(), 0);
		vkDestroyDevice(m_device, nullptr);
	}

	bool GraphicsDevice_Vulkan::pickPhysicalDevice()
	{
		SP_PROFILE_FUNCTION();

		uint32_t physicalDevicesCount;
		vkEnumeratePhysicalDevices(*m_instance, &physicalDevicesCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
		vkEnumeratePhysicalDevices(*m_instance, &physicalDevicesCount, physicalDevices.data());
		spdlog::info("There {} {} physical device{} available:", physicalDevicesCount > 1 ? "are" : "is",
		             physicalDevicesCount, physicalDevicesCount > 1 ? "s" : "");
		for (VkPhysicalDevice& pDevice : physicalDevices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(pDevice, &properties);
			if (!(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || properties.deviceType ==
				VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
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
			//for (auto& surface : m_desc.surfaces)
			//{
			//	Ref<GraphicsSurface_Vulkan> surface_vk = std::static_pointer_cast<GraphicsSurface_Vulkan>(
			//		surface->internal_state);
			//	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pDevice, surface_vk.get());
			//	swapChainAdequate &= !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			//}
			if (!supportExt || !swapChainAdequate)
				continue;
			spdlog::info("  Device \"{}\" ({}) --> {} available extensions", properties.deviceName,
			             getPhysicalDeviceTypeName(properties.deviceType), extCount);
			m_physicalDevice = pDevice;
			//msaaSamples = getMaxUsableSampleCount();
			return true;
		}
		if (!m_physicalDevice)
			SPRING_ERROR("Can't find suitable physical device!")
		return false;
	}

	bool GraphicsDevice_Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		SP_PROFILE_FUNCTION();

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceRequiredExtensions.begin(), m_deviceRequiredExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails GraphicsDevice_Vulkan::querySwapChainSupport(
		VkPhysicalDevice device, VkSurfaceKHR& surface)
	{
		SP_PROFILE_FUNCTION();

		SwapChainSupportDetails details = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
			                                          details.presentModes.data());
		}
		return details;
	}

	AllocationHandler* GraphicsDevice_Vulkan::getAllocationHandler() const
	{
		return m_allocationHandler.get();
	}

	QueueFamilyIndices GraphicsDevice_Vulkan::findQueueFamilies(VkPhysicalDevice device) const
	{
		SP_PROFILE_FUNCTION();

		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			if (m_desc.supportPresent)
			{
				//if (m_desc.surfaces.size() <= 0)
				//	spdlog::error("Shouldn't be here...");
				VkBool32 presentSupport = true; // TODO
				//VkSurfaceKHR surface = std::static_pointer_cast<GraphicsSurface_Vulkan>(
				//	m_desc.surfaces[0]->internal_state)->surface;
				//vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
				}
			}

			i++;
		}

		return indices;
	}

	bool GraphicsDevice_Vulkan::createDevice()
	{
		SP_PROFILE_FUNCTION();

		m_queueFamilies = findQueueFamilies(m_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_queueFamilies.graphicsFamily.value(), m_queueFamilies.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
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
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(m_deviceRequiredExtensions.size()),
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
		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_device, m_queueFamilies.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_queueFamilies.presentFamily.value(), 0, &m_presentQueue);
		return true;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		SP_PROFILE_FUNCTION();

		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace ==
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		SP_PROFILE_FUNCTION();

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
		//return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}

	VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates,
	                             VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (const VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
	{
		return findSupportedFormat(physicalDevice,
		                           {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		                           VK_IMAGE_TILING_OPTIMAL,
		                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
	                            uint32_t mipLevels)
	{
		const VkImageViewCreateInfo viewInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.subresourceRange
			{
				.aspectMask = aspectFlags,
				.baseMipLevel = 0,
				.levelCount = mipLevels,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	bool GraphicsDevice_Vulkan::createSwapChain(SwapChainDesc& desc, SwapChain* swapchain)
	{
		SP_PROFILE_FUNCTION();

		auto internal_state = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);
		if (!swapchain->isValid())
		{
			internal_state = makeRef<SwapChain_Vulkan>();
		}
		internal_state->allocationHandler = m_allocationHandler;
		swapchain->desc = desc;
		swapchain->internal_state = internal_state;

		if (desc.presentWindow && internal_state->surface == VK_NULL_HANDLE)
		{
			internal_state->hasSurface = true;
			internal_state->surface = std::static_pointer_cast<GraphicsSurface_Vulkan>(desc.window->getSurface()->internal_state)->surface;
		}

		//Swapchain support
		//SwapChainSupportDetails swapChainSupport = querySwapChainSupport(
			//m_physicalDevice, std::static_pointer_cast<GraphicsSurface_Vulkan>(desc.window->get->internal_state).get());
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, internal_state->surface);
		//SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, VK_NULL_HANDLE);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = {};

		if (swapChainSupport.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = swapChainSupport.capabilities.currentExtent;
		}
		else
		{
			extent = {
				desc.width,
				desc.height
			};

			extent.width = std::clamp(extent.width, swapChainSupport.capabilities.minImageExtent.width,
			                          swapChainSupport.capabilities.maxImageExtent.width);
			extent.height = std::clamp(extent.height, swapChainSupport.capabilities.minImageExtent.height,
			                           swapChainSupport.capabilities.maxImageExtent.height);
		}

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		//desc.hasSurface;
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
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = internal_state->swapchain;
		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &internal_state->swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// Get images
		vkGetSwapchainImagesKHR(m_device, internal_state->swapchain, &imageCount, nullptr);
		internal_state->swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, internal_state->swapchain, &imageCount,
		                        internal_state->swapchainImages.data());
		internal_state->swapChainImageFormat = surfaceFormat.format;
		internal_state->swapChainExtent = extent;

		// Create imageviews
		internal_state->swapchainImageViews.resize(internal_state->swapchainImages.size());

		for (uint32_t i = 0; i < internal_state->swapchainImages.size(); i++)
		{
			internal_state->swapchainImageViews[i] = createImageView(m_device, internal_state->swapchainImages[i],
			                                                         internal_state->swapChainImageFormat,
			                                                         VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}

		// Create renderpass
		VkAttachmentDescription colorAttachment
		{
			.format = internal_state->swapChainImageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT, // To change for multisampling
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference colorAttachmentRef
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		/*
		VkAttachmentDescription depthAttachment
		{
			.format = findDepthFormat(m_physicalDevice),
			.samples = VK_SAMPLE_COUNT_1_BIT, // To change for multisampling
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference depthAttachmentRef
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
		*/

		/*
		VkAttachmentDescription colorAttachmentResolve
		{
			.format = internal_state->swapChainImageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentResolveRef
		{
			.attachment = 2,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		*/

		VkSubpassDescription subpass
		{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentRef,
			//.pResolveAttachments = &colorAttachmentResolveRef,
			//.pDepthStencilAttachment = &depthAttachmentRef
		};

		VkSubpassDependency dependency
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
		};

		//std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		//std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		std::array<VkAttachmentDescription, 1> attachments = {colorAttachment};

		VkRenderPassCreateInfo renderPassInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency
		};
		Ref<RenderPass_Vulkan> renderpass_internal = makeRef<RenderPass_Vulkan>();
		renderpass_internal->allocationHandler = m_allocationHandler;
		internal_state->renderpass.internal_state = renderpass_internal;
		// TODO : Attachments
		if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &renderpass_internal->renderpass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}

		internal_state->swapchainFramebuffers.resize(internal_state->swapchainImageViews.size());

		for (size_t i = 0; i < internal_state->swapchainImageViews.size(); i++)
		{
			std::vector<VkImageView> framebufferAttachments = {
				internal_state->swapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = renderpass_internal->renderpass,
				.attachmentCount = static_cast<uint32_t>(attachments.size()),
				.pAttachments = framebufferAttachments.data(),
				.width = internal_state->swapChainExtent.width,
				.height = internal_state->swapChainExtent.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &internal_state->swapchainFramebuffers[i]) !=
				VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}

		return true;
	}


	bool GraphicsDevice_Vulkan::createGraphicsPipeline(GraphicsPipelineDesc& desc, GaphicsPipeline* swapchain)
	{
		SP_PROFILE_FUNCTION();
		return true;
	}

	bool GraphicsDevice_Vulkan::createShader(ShaderDesc& desc, Shader* shader)
	{
		SP_PROFILE_FUNCTION();
		const Ref<Shader_Vulkan> internal_state = makeRef<Shader_Vulkan>();
		internal_state->allocationHandler = m_allocationHandler;
		shader->internal_state = internal_state;
		shader->stage = desc.stage;

		const VkShaderModuleCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = desc.size,
			.pCode = reinterpret_cast<const uint32_t*>(desc.source)
		};

		if (vkCreateShaderModule(m_device, &createInfo, nullptr, &internal_state->shaderModule) != VK_SUCCESS)
			SPRING_ERROR("Failed to create shader module")

		internal_state->shaderStageInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.module = internal_state->shaderModule,
			.pName = "main"
		};
		switch (desc.stage)
		{
		case ShaderStage::Vertex:
			internal_state->shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ShaderStage::Fragment:
			internal_state->shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case ShaderStage::None:
			return false;
		}
		return true;
	}
}
