//#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>
#include "SpringGraphicsDevice_Vulkan.hpp"

#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringCore/SpringCommon.hpp>

#include <Spring/SpringGraphics/SpringGraphicsCommon.hpp>
#include <Spring/SpringGraphics/ISpringWindow.hpp>
//#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"
#include <Spring/SpringGraphics/SpringGraphicsVulkanUtils.hpp>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

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
        VkSemaphore acquire_semaphore = VK_NULL_HANDLE;
        VkSemaphore release_semaphore = VK_NULL_HANDLE;
        uint32_t currentImageIndex = 0;

		RenderPass renderpass = {};

		~SwapChain_Vulkan()
		{
            if (allocationHandler == nullptr)
                return;
            std::lock_guard locker(allocationHandler->updateLock);
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
        Ref<AllocationHandler> allocationHandler;
		VkViewport viewport = {};
		VkRect2D scissor = {};

        VkGraphicsPipelineCreateInfo createInfo = {};
        VkPipelineShaderStageCreateInfo shaderStages[(size_t)(ShaderStage::None)] = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        VkPipelineViewportStateCreateInfo viewportState = {};

        std::vector<VkVertexInputBindingDescription> bindingsDescription = {};
        std::vector<VkVertexInputAttributeDescription> attributesDescription = {};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

        VkPipelineDynamicStateCreateInfo dynamicState = {};
        std::vector<VkDynamicState> dynamicStates = {};

        size_t hash = {};

        ~GraphicsPipeline_Vulkan()
        {
            if (allocationHandler == nullptr)
                return;
            std::lock_guard locker(allocationHandler->updateLock);
            if(pipelineLayout)
                allocationHandler->pipelineLayouts.emplace_back(pipelineLayout, allocationHandler->framecount);
            if(pipeline)
                allocationHandler->pipelines.emplace_back(pipeline, allocationHandler->framecount);
        }
	};

    struct Buffer_Vulkan {
        Ref<AllocationHandler> allocationHandler = nullptr;
        VmaAllocation allocation = nullptr;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceAddress address = 0;

        ~Buffer_Vulkan() {
            std::lock_guard guard(allocationHandler->updateLock);
            if(buffer) {
                allocationHandler->buffers.emplace_back(std::make_pair(buffer, allocation), allocationHandler->framecount);
            }
        }
    };

    struct Texture_Vulkan {
        Ref<AllocationHandler> allocationHandler = nullptr;
        VmaAllocation allocation = nullptr;
        VkImage texture = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
    };

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

    struct CommandBuffer_Vulkan {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers = {};
        uint32_t currentIndex = 0;
    };

    struct CommandList_Vulkan
    {
        GraphicsPipeline* pipeline = nullptr;
        RenderPass* renderpass = nullptr;
        //std::vector<SwapChain> swapchains = {};
        Ref<SwapChain> swapchain;
    };

    constexpr VkFormat getVkFormat(Format f) {
        switch(f) {
            // R
            case Format::R_32_SFLOAT:   return VK_FORMAT_R32_SFLOAT;
            case Format::R_32_SINT:     return VK_FORMAT_R32_SINT;
            case Format::R_32_UINT:     return VK_FORMAT_R32_UINT;
            case Format::R_16_SFLOAT:   return VK_FORMAT_R16_SFLOAT;
            case Format::R_16_UNORM:    return VK_FORMAT_R16_UNORM;
            case Format::R_16_SNORM:    return VK_FORMAT_R16_SNORM;
            case Format::R_16_SINT:     return VK_FORMAT_R16_SINT;
            case Format::R_16_UINT:     return VK_FORMAT_R16_UINT;
            case Format::R_8_SRGB:      return VK_FORMAT_R8_SRGB;
            case Format::R_8_UNORM:     return VK_FORMAT_R8_UNORM;
            case Format::R_8_SNORM:     return VK_FORMAT_R8_SNORM;
            case Format::R_8_UINT:      return VK_FORMAT_R8_UINT;
            case Format::R_8_SINT:      return VK_FORMAT_R8_SINT;

            case Format::RG_32_SFLOAT:  return VK_FORMAT_R32G32_SFLOAT;
            case Format::RG_32_SINT:    return VK_FORMAT_R32G32_SINT;
            case Format::RG_32_UINT:    return VK_FORMAT_R32G32_UINT;
            case Format::RG_16_SFLOAT:  return VK_FORMAT_R16G16_SFLOAT;
            case Format::RG_16_UNORM:   return VK_FORMAT_R16G16_UNORM;
            case Format::RG_16_SNORM:   return VK_FORMAT_R16G16_SNORM;
            case Format::RG_16_SINT:    return VK_FORMAT_R16G16_SINT;
            case Format::RG_16_UINT:    return VK_FORMAT_R16G16_UINT;
            case Format::RG_8_SRGB:     return VK_FORMAT_R8G8_SRGB;
            case Format::RG_8_UNORM:    return VK_FORMAT_R8G8_UNORM;
            case Format::RG_8_SNORM:    return VK_FORMAT_R8G8_SNORM;
            case Format::RG_8_UINT:     return VK_FORMAT_R8G8_UINT;
            case Format::RG_8_SINT:     return VK_FORMAT_R8G8_SINT;

            // RGB
            case Format::RGB_32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
            case Format::RGB_32_SINT:   return VK_FORMAT_R32G32B32_SINT;
            case Format::RGB_32_UINT:   return VK_FORMAT_R32G32B32_UINT;
            case Format::RGB_16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
            case Format::RGB_16_UNORM:  return VK_FORMAT_R16G16B16_UNORM;
            case Format::RGB_16_SNORM:  return VK_FORMAT_R16G16B16_SNORM;
            case Format::RGB_16_SINT:   return VK_FORMAT_R16G16B16_SINT;
            case Format::RGB_16_UINT:   return VK_FORMAT_R16G16B16_UINT;
            case Format::RGB_8_SRGB:    return VK_FORMAT_R8G8B8_SRGB;
            case Format::RGB_8_UNORM:   return VK_FORMAT_R8G8B8_UNORM;
            case Format::RGB_8_SNORM:   return VK_FORMAT_R8G8B8_SNORM;
            case Format::RGB_8_SINT:    return VK_FORMAT_R8G8B8_SINT;
            case Format::RGB_8_UINT:    return VK_FORMAT_R8G8B8_UINT;

            // RGBA
            case Format::RGBA_32_SFLOAT:return VK_FORMAT_R32G32B32A32_SFLOAT;
            case Format::RGBA_32_SINT:  return VK_FORMAT_R32G32B32A32_SINT;
            case Format::RGBA_32_UINT:  return VK_FORMAT_R32G32B32A32_UINT;
            case Format::RGBA_16_SFLOAT:return VK_FORMAT_R16G16B16A16_SFLOAT;
            case Format::RGBA_16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
            case Format::RGBA_16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
            case Format::RGBA_16_SINT:  return VK_FORMAT_R16G16B16A16_SINT;
            case Format::RGBA_16_UINT:  return VK_FORMAT_R16G16B16A16_UINT;
            case Format::RGBA_8_SRGB:   return VK_FORMAT_R8G8B8A8_SRGB;
            case Format::RGBA_8_UNORM:  return VK_FORMAT_R8G8B8A8_UNORM;
            case Format::RGBA_8_SNORM:  return VK_FORMAT_R8G8B8A8_SNORM;
            case Format::RGBA_8_UINT:   return VK_FORMAT_R8G8B8A8_UINT;
            case Format::RGBA_8_SINT:   return VK_FORMAT_R8G8B8A8_SINT;


            case Format::Empty:
            default:            return VK_FORMAT_UNDEFINED;
        }
    }

    constexpr uint32_t getStride(Format f) {
        switch (f) {
            // R
            case Format::R_32_SFLOAT:
            case Format::R_32_SINT:
            case Format::R_32_UINT:     return 4u;
            case Format::R_16_SFLOAT:
            case Format::R_16_UNORM:
            case Format::R_16_SNORM:
            case Format::R_16_SINT:
            case Format::R_16_UINT:     return 2u;
            case Format::R_8_SRGB:
            case Format::R_8_UNORM:
            case Format::R_8_SNORM:
            case Format::R_8_UINT:
            case Format::R_8_SINT:      return 1u;

            // RG
            case Format::RG_32_SFLOAT:
            case Format::RG_32_SINT:
            case Format::RG_32_UINT:    return 8u;
            case Format::RG_16_SFLOAT:
            case Format::RG_16_UNORM:
            case Format::RG_16_SNORM:
            case Format::RG_16_SINT:
            case Format::RG_16_UINT:    return 4u;
            case Format::RG_8_SRGB:
            case Format::RG_8_UNORM:
            case Format::RG_8_SNORM:
            case Format::RG_8_UINT:
            case Format::RG_8_SINT:     return 2u;

            // RGB
            case Format::RGB_32_SFLOAT:
            case Format::RGB_32_SINT:
            case Format::RGB_32_UINT:   return 12u;
            case Format::RGB_16_SFLOAT:
            case Format::RGB_16_UNORM:
            case Format::RGB_16_SNORM:
            case Format::RGB_16_SINT:
            case Format::RGB_16_UINT:   return 6u;
            case Format::RGB_8_SRGB:
            case Format::RGB_8_UNORM:
            case Format::RGB_8_SNORM:
            case Format::RGB_8_SINT:
            case Format::RGB_8_UINT:    return 3u;

            // RGBA
            case Format::RGBA_32_SFLOAT:
            case Format::RGBA_32_SINT:
            case Format::RGBA_32_UINT:  return 16u;
            case Format::RGBA_16_SFLOAT:
            case Format::RGBA_16_UNORM:
            case Format::RGBA_16_SNORM:
            case Format::RGBA_16_SINT:
            case Format::RGBA_16_UINT:  return 8u;
            case Format::RGBA_8_SRGB:
            case Format::RGBA_8_UNORM:
            case Format::RGBA_8_SNORM:
            case Format::RGBA_8_UINT:
            case Format::RGBA_8_SINT:   return 4u;


            case Format::Empty:
            default:                    return 4u;
        }
    }

    void CopyHandler::init(GraphicsDevice_Vulkan* gd) {
        SP_PROFILE_FUNCTION();

        device = gd;
        VkCommandPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = device->m_queueFamilies.graphicsFamily.value(),
        };

        vkCreateCommandPool(device->m_device, &createInfo, nullptr, &commandPool);

        // HERE --> VkTimelineSemaphoreSubmitInfo TODO
        VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .pNext = nullptr,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };

        VkSemaphoreCreateInfo semaphoreCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = &semaphoreTypeCreateInfo,
                .flags = 0,
        };
        vkCreateSemaphore(device->m_device, &semaphoreCreateInfo, nullptr, &semaphore);
    }

    CopyHandler::CopyTask CopyHandler::allocate(size_t size) { // NOLINT(misc-no-recursion)
        if(freeTasks.empty()) {
            auto task = CopyTask{};

            VkCommandBufferAllocateInfo allocateInfo = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .commandPool = commandPool,
                    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    .commandBufferCount = 1,
            };

            vkAllocateCommandBuffers(device->m_device, &allocateInfo, &task.cmd);

            freeTasks.push_back(task);
        }

        CopyTask task = freeTasks.back();
        if(task.buffer.desc.size<size) {
            for(auto& t : freeTasks) {
                if(t.buffer.desc.size>=size) {
                    task = t;
                    std::swap(task, freeTasks.back());
                }
            }
        }
        freeTasks.pop_back();

        if(task.buffer.desc.size<size) {
            device->createBuffer({
                .size = (uint32_t)size,
            }, &task.buffer, nullptr);
        }

        vkResetCommandPool(device->m_device, commandPool, 0); // TODO : have a command pool per ongoing copy command \
                                                              // as every buffer has to be not in use for this command

        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };

        vkBeginCommandBuffer(task.cmd, &beginInfo);

        return task;
    }

    void CopyHandler::submit(const CopyHandler::CopyTask& task) {
        SP_PROFILE_FUNCTION();

        vkEndCommandBuffer(task.cmd);

        ongoing.push_back(task);
        toSubmit.push_back(task.cmd);
    }

    void CopyHandler::performCopy() {
        SP_PROFILE_FUNCTION();

        if (!toSubmit.empty())
        {
            VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .commandBufferCount = (uint32_t)toSubmit.size(),
                .pCommandBuffers = toSubmit.data(),
                .signalSemaphoreCount = 0,
                .pSignalSemaphores = nullptr//&semaphore,
            };

            VkTimelineSemaphoreSubmitInfo timelineInfo = {
                .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .waitSemaphoreValueCount = 0,
                .pWaitSemaphoreValues = nullptr,
                .signalSemaphoreValueCount = 1,
                .pSignalSemaphoreValues = &count,
            };

            submitInfo.pNext = &timelineInfo;

            VkResult res = vkQueueSubmit(device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE); // TODO : copy queue
            assert(res == VK_SUCCESS);

            toSubmit.clear();
        }

        count = 0;
    }

	GraphicsDevice_Vulkan::GraphicsDevice_Vulkan(GraphicsDeviceDesc desc, SpringGraphicsApi* api) :
		GraphicsDevice(std::move(desc), api), m_device(nullptr),
		m_allocationHandler(makeRef<AllocationHandler>()),
        m_copyHandler(makeRef<CopyHandler>()),
        m_pipelines{}
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
			//m_deviceRequiredExtensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
		}
		if (!pickPhysicalDevice())
		{
			spdlog::error("Unable to find suitable GPU...");
		}
		createDevice();
		m_allocationHandler->device = m_device;

        VmaVulkanFunctions vma_vulkan_func{};
        vma_vulkan_func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vma_vulkan_func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
        vma_vulkan_func.vkFreeMemory = vkFreeMemory;
        vma_vulkan_func.vkMapMemory = vkMapMemory;
        vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
        vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
        vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
        vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
        vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
        vma_vulkan_func.vkCreateImage = vkCreateImage;
        vma_vulkan_func.vkDestroyImage = vkDestroyImage;
        vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_physicalDevice;
        allocatorInfo.device = m_device;
        allocatorInfo.instance = *m_instance;

        // Core in 1.1
        allocatorInfo.flags =
                VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT |
                VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
        vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
        vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;

        //allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vma_vulkan_func.vkBindBufferMemory2KHR = vkBindBufferMemory2;
        vma_vulkan_func.vkBindImageMemory2KHR = vkBindImageMemory2;

        allocatorInfo.pVulkanFunctions = &vma_vulkan_func;

        VkResult res = vmaCreateAllocator(&allocatorInfo, &m_allocationHandler->allocator);
        if(res!=VK_SUCCESS)
            core::error("Failed to initialize vma");

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

        m_copyHandler->init(this);
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

        // Destruction is now handled by AllocationHandler
		//m_allocationHandler->update(std::numeric_limits<uint64_t>::max(), 0);
		//vkDestroyDevice(m_device, nullptr);
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

		VkPhysicalDeviceFeatures deviceFeatures{
            .samplerAnisotropy = VK_TRUE,
		};

        physicalDeviceFeatures12 = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                .timelineSemaphore = VK_TRUE,
                //.bufferDeviceAddress = VK_TRUE,
                //.bufferDeviceAddressCaptureReplay = VK_TRUE,
        };

        VkDeviceCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &physicalDeviceFeatures12,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(m_deviceRequiredExtensions.size()),
			.ppEnabledExtensionNames = m_deviceRequiredExtensions.data(),
			.pEnabledFeatures = &deviceFeatures,
		};
        //createInfo.pNext->PhysicalDeviceFeatures2.pNext->PhysicalDeviceTimelineSemaphoreFeatures.timelineSemaphore = true

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

        volkLoadDevice(m_device);

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
        SP_PROFILE_FUNCTION();

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
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

		VkAttachmentReference colorAttachmentRef
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		//VkAttachmentDescription depthAttachment
		//{
		//	.format = findDepthFormat(m_physicalDevice),
		//	.samples = VK_SAMPLE_COUNT_1_BIT, // To change for multisampling
		//	.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//	.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//	.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		//	.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//	.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//	.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		//};
        //
		//VkAttachmentReference depthAttachmentRef
		//{
		//	.attachment = 1,
		//	.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		//};
        //
		//VkAttachmentDescription colorAttachmentResolve
		//{
		//	.format = internal_state->swapChainImageFormat,
		//	.samples = VK_SAMPLE_COUNT_1_BIT,
		//	.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		//	.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//	.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		//	.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//	.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//	.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		//};
        //
		//VkAttachmentReference colorAttachmentResolveRef
		//{
		//	.attachment = 2,
		//	.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//};

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
		std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };

		VkRenderPassCreateInfo renderPassInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency,
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
				.attachmentCount = static_cast<uint32_t>(framebufferAttachments.size()),
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

        VkSemaphoreCreateInfo semaphoreCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &internal_state->acquire_semaphore);
        vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &internal_state->release_semaphore);

		return true;
	}

    bool GraphicsDevice_Vulkan::createRenderPass(spring::graphics::RenderPassDesc &desc,
                                                 spring::graphics::RenderPass *renderpass) {
        SP_PROFILE_FUNCTION();

        if(renderpass->isValid())
            spdlog::error("Attempting to override an already valid renderpass");
        auto internal_state = makeRef<RenderPass_Vulkan>();
        internal_state->allocationHandler = m_allocationHandler;
        renderpass->internal_state = internal_state;
        renderpass->desc = desc;

        std::vector<VkAttachmentDescription2> attachmentDescriptions;

        for(RenderPassAttachment& attachment : desc.attachments)
        {
            Texture* texture = attachment.texture;
            TextureDesc textureDesc = texture->desc;
            Ref<Texture_Vulkan> texture_internal = std::static_pointer_cast<Texture_Vulkan>(texture->internal_state);

            attachmentDescriptions.push_back({
                .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
                .format = graphics::getVkFormat(textureDesc.format),
                .samples = VK_SAMPLE_COUNT_8_BIT,
            });
            VkAttachmentDescription2& description = attachmentDescriptions.back();

            switch (attachment.loadOp) {
                case RenderPassAttachment::LoadOp::Load:
                    description.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                    break;
                case RenderPassAttachment::LoadOp::Clear:
                    description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    break;
                case RenderPassAttachment::LoadOp::DontCare:
                    description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    break;
            }

            switch (attachment.storeOp) {
                case RenderPassAttachment::StoreOp::Store:
                    description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    break;
                case RenderPassAttachment::StoreOp::DontCare:
                    description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    break;
            }

            if(attachment.type==RenderPassAttachment::Type::RenderTarget)
            {
                //attachment.
            }
        }
        return true;
    }

	bool GraphicsDevice_Vulkan::createGraphicsPipeline(GraphicsPipelineDesc& desc, GraphicsPipeline* pipeline)
	{
		SP_PROFILE_FUNCTION();

        auto internal_state = std::static_pointer_cast<GraphicsPipeline_Vulkan>(pipeline->internal_state);
        if (!pipeline->isValid())
        {
            internal_state = makeRef<GraphicsPipeline_Vulkan>();
        }
        pipeline->internal_state = internal_state;
        internal_state->allocationHandler = m_allocationHandler;
        pipeline->desc = desc;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = std::static_pointer_cast<Shader_Vulkan>(desc.vs->internal_state)->shaderModule,
            .pName = "main"
        };

        VkPipelineShaderStageCreateInfo fragShaderStageInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = std::static_pointer_cast<Shader_Vulkan>(desc.fs->internal_state)->shaderModule,
            .pName = "main"
        };

        VkPipelineShaderStageCreateInfo (&shaderStages)[] = internal_state->shaderStages;
        shaderStages[0] = vertShaderStageInfo;
        shaderStages[1] = fragShaderStageInfo;

        std::vector<VkVertexInputBindingDescription>& bindings = internal_state->bindingsDescription;
        std::vector<VkVertexInputAttributeDescription>& attributesDescription = internal_state->attributesDescription;

        std::vector<uint32_t> strides = {};

        for(auto& input : desc.inputLayout.inputs)
        {
            while(input.binding>=bindings.size()) {
                VkVertexInputBindingDescription& b = bindings.emplace_back();
                b.binding = uint32_t(bindings.size())-1;
                b.inputRate = VK_VERTEX_INPUT_RATE_MAX_ENUM;
                strides.push_back(0);
            }
            VkVertexInputBindingDescription& binding = bindings[input.binding];
            uint32_t& stride = strides[input.binding];
            VkVertexInputRate rate = input.inputType==InputRates::Vertex ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;

            stride+=getStride(input.format);
            if((binding.inputRate!=VK_VERTEX_INPUT_RATE_MAX_ENUM) && binding.inputRate!=rate) {
                spdlog::error("Can't assign input rate ({}) to binding ({}) with rate ({})", rate, input.binding, binding.inputRate);
                return false;
            }
            binding.inputRate = rate;
            binding.stride = stride;

            VkVertexInputAttributeDescription attribute = {
                    .location = input.location,
                    .binding = 0,
                    .format = getVkFormat(input.format),
                    .offset = input.attributeOffset
            };
            attributesDescription.emplace_back(attribute);
        }

        VkPipelineVertexInputStateCreateInfo& vertexInputInfo = internal_state->vertexInputInfo;
        vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = (uint32_t)bindings.size(),
            .pVertexBindingDescriptions = bindings.data(),
            .vertexAttributeDescriptionCount = (uint32_t)attributesDescription.size(),
            .pVertexAttributeDescriptions = attributesDescription.data()
        };

        VkPipelineInputAssemblyStateCreateInfo& inputAssembly = internal_state->inputAssembly;
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport& viewport = internal_state->viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = 16384; // Currently max device width (16384), adjusted on rendering
        viewport.height = 16384;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D& scissor = internal_state->scissor;
        scissor.extent.width  = INT32_MAX;
        scissor.extent.height = INT32_MAX;

        VkPipelineViewportStateCreateInfo& viewportState = internal_state->viewportState;
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo& rasterizer = internal_state->rasterizer;
        rasterizer = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo& multisampling = internal_state->multisampling;
        multisampling = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE
        };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            //.pSetLayouts = &internal_state->descriptorSetLayout,
            .pushConstantRangeCount = 0,
        };

        if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &internal_state->pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        std::vector<VkDynamicState>& dynamicStates = internal_state->dynamicStates;
        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

        VkPipelineDynamicStateCreateInfo& dynamicState = internal_state->dynamicState;
        dynamicState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };

        internal_state->createInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr, // Set on validation
            .pColorBlendState = nullptr, // Set on validation
            .pDynamicState = &dynamicState,
            .layout = internal_state->pipelineLayout,
            .renderPass = nullptr, // Set on validation
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        //VkResult res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &internal_state->pipeline);
		//return res == VK_SUCCESS;

        return true;
	}

    bool GraphicsDevice_Vulkan::validatePipeline(const Ref<CommandList>& cmd) {
        SP_PROFILE_FUNCTION();

        auto cmd_internal = std::static_pointer_cast<CommandList_Vulkan>(cmd->internal_state);
        if (!cmd->isValid())
            core::error("Invalid command list");
        auto pipeline_internal = std::static_pointer_cast<GraphicsPipeline_Vulkan>(cmd_internal->pipeline->internal_state);
        VkResult res = VK_SUCCESS;
        if(!pipeline_internal->pipeline)
        {
            VkGraphicsPipelineCreateInfo* createInfo = &pipeline_internal->createInfo;

            VkPipelineColorBlendAttachmentState colorBlendAttachment
            {
                .blendEnable = VK_FALSE,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
            };

            VkPipelineColorBlendStateCreateInfo colorBlending
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = 1,
                .pAttachments = &colorBlendAttachment,
                .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
                //.blendConstants[1] = 0.0f,
                // .blendConstants[2] = 0.0f,
                // .blendConstants[3] = 0.0f
            };

            VkPipelineDepthStencilStateCreateInfo depthStencil
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
                .front = {},
                .back = {},
                .minDepthBounds = 0.0f,
                .maxDepthBounds = 1.0f
            };

            createInfo->renderPass = std::static_pointer_cast<RenderPass_Vulkan>(cmd_internal->renderpass->internal_state)->renderpass;
            createInfo->pColorBlendState = &colorBlending;
            createInfo->pDepthStencilState = &depthStencil;

            res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, createInfo, nullptr, &pipeline_internal->pipeline);
        }
        return res==VK_SUCCESS;
    }

    bool GraphicsDevice_Vulkan::createBuffer(BufferDesc desc, Buffer *buffer, void* data) {
        SP_PROFILE_FUNCTION();

        auto internal_state = makeRef<Buffer_Vulkan>();
        internal_state->allocationHandler = m_allocationHandler;
        buffer->internal_state = internal_state;
        buffer->desc = desc;

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VkBufferCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = desc.size,
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VmaAllocationCreateInfo allocInfo = {
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
        };

        vmaCreateBuffer(internal_state->allocationHandler->allocator, &createInfo, &allocInfo, &internal_state->buffer, &internal_state->allocation, nullptr);

        if(data!=nullptr) {
            auto task = m_copyHandler->allocate(desc.size);
            auto staging = std::static_pointer_cast<Buffer_Vulkan>(task.buffer.internal_state);

            void* mappedData;
            vmaMapMemory(internal_state->allocationHandler->allocator, staging->allocation, &mappedData);
            //vkMapMemory(m_device, staging->buffer, 0, sizeof(data), 0, &data);
            memcpy(mappedData, data, desc.size);
            vmaUnmapMemory(internal_state->allocationHandler->allocator, staging->allocation);

            VkBufferCopy copyRegion
            {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = desc.size
            };

            vkCmdCopyBuffer(task.cmd, staging->buffer, internal_state->buffer, 1, &copyRegion);

            m_copyHandler->submit(task);
            m_copyHandler->performCopy(); // TODO : batch the call
        }

        return true;
    }

    bool GraphicsDevice_Vulkan::createTexture(spring::graphics::TextureDesc desc, spring::graphics::Texture *texture) {
        SP_PROFILE_FUNCTION();

        auto internal_state = makeRef<Texture_Vulkan>();
        internal_state->allocationHandler = m_allocationHandler;
        texture->internal_state = internal_state;
        texture->desc = desc;

        if (desc.mipLevels == 0)
            desc.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;

        VkImageCreateInfo imageInfo
                {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .flags = 0,
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = getVkFormat(texture->desc.format),
                        .extent
                                {
                                        .width = texture->desc.width,
                                        .height = texture->desc.height,
                                        .depth = 1
                                },
                        .mipLevels = texture->desc.mipLevels,
                        .arrayLayers = 1,
                        .samples = static_cast<VkSampleCountFlagBits>(texture->desc.samples),
                        .tiling = VK_IMAGE_TILING_OPTIMAL,
                        .usage = 0,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
                };

        if (desc.usage == Usage::Default) {
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        //if (vkCreateImage(m_device, &imageInfo, nullptr, &internal_state->texture) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to create image!");
        //}
        //
        //VkMemoryRequirements memRequirements;
        //vkGetImageMemoryRequirements(m_device, internal_state->texture, &memRequirements);
        //
        //VkMemoryAllocateInfo allocInfo
        //{
        //    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        //    .allocationSize = memRequirements.size,
        //    .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        //};

        VmaAllocationCreateInfo allocInfo = {
            .usage = VMA_MEMORY_USAGE_AUTO,
        };

        VkResult res = vmaCreateImage(m_allocationHandler->allocator, &imageInfo, &allocInfo, &internal_state->texture, &internal_state->allocation, nullptr);

        return true;
    }

    uint32_t GraphicsDevice_Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    bool GraphicsDevice_Vulkan::createShader(ShaderDesc desc, Shader* shader)
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

    bool GraphicsDevice_Vulkan::createCommandBuffer(const Ref<CommandBuffer>& commandBuffer) {
        SP_PROFILE_FUNCTION();

        auto internal_state = std::static_pointer_cast<CommandBuffer_Vulkan>(commandBuffer->internal_state);
        if(!commandBuffer->isValid())
            internal_state = makeRef<CommandBuffer_Vulkan>();
        commandBuffer->internal_state = internal_state;
        internal_state->commandBuffers.resize(GraphicsDevice::framesInFlight);

        VkCommandPoolCreateInfo cmdPoolInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        };

        switch (commandBuffer->queueType) {
            case Graphics:
                cmdPoolInfo.queueFamilyIndex = m_queueFamilies.graphicsFamily.value();
                break;
            case Transfert:
                //cmdPoolInfo.queueFamilyIndex = TODO
                break;
            case Protected:
                // TODO
                break;
            default:
                break;
        }

        vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &internal_state->commandPool);

        VkCommandBufferAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = internal_state->commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = (uint32_t)internal_state->commandBuffers.size(),
        };

        vkAllocateCommandBuffers(m_device, &allocInfo, internal_state->commandBuffers.data());
        return true;
    }

    bool GraphicsDevice_Vulkan::bindPipeline(const Ref<CommandList>& cmd, const Ref<GraphicsPipeline>& pipeline) {
        SP_PROFILE_FUNCTION();

        std::static_pointer_cast<CommandList_Vulkan>(cmd->internal_state)->pipeline = pipeline.get();
        return true;
    }

    bool GraphicsDevice_Vulkan::bindVertexBuffer(const Ref<CommandList>& cmd, const Ref<Buffer>& buffer, uint32_t first) {
        SP_PROFILE_FUNCTION();

        auto internal_state = std::static_pointer_cast<Buffer_Vulkan>(buffer->internal_state);
        auto commandBuffer = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        VkBuffer buffers[] = { internal_state->buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer->commandBuffers[commandBuffer->currentIndex], first, 1, buffers, offsets);

        return true;
    }

    bool GraphicsDevice_Vulkan::beginCommandList(const Ref<CommandList>& cmd) {
        SP_PROFILE_FUNCTION();

        vkDeviceWaitIdle(m_device);
        auto commandList = std::static_pointer_cast<CommandList_Vulkan>(cmd->internal_state);
        if(!cmd->isValid()) {
            commandList = makeRef<CommandList_Vulkan>();
        }
        cmd->internal_state = commandList;
        auto commandBuffer = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        VkCommandBufferBeginInfo beginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        VkResult res = vkResetCommandPool(m_device, commandBuffer->commandPool, 0);

        vkBeginCommandBuffer(commandBuffer->commandBuffers[commandBuffer->currentIndex], &beginInfo); // TODO -> is it increased?

        return true;
    }

    bool GraphicsDevice_Vulkan::beginRenderPass(const Ref<CommandList>& cmd, const Ref<SwapChain>& swapchain) {
        SP_PROFILE_FUNCTION();

        auto commandList = std::static_pointer_cast<CommandList_Vulkan>(cmd->internal_state);
        auto swapchain_internal = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);

        commandList->swapchain = swapchain;
        vkDeviceWaitIdle(m_device);
        {
            SP_PROFILE_SCOPE("AcquireNextImageKHR");
            VkResult res = vkAcquireNextImageKHR(m_device, swapchain_internal->swapchain, UINT64_MAX,
                                                 swapchain_internal->acquire_semaphore, VK_NULL_HANDLE,
                                                 &swapchain_internal->currentImageIndex);
            if (res == VK_ERROR_OUT_OF_DATE_KHR) {
                spdlog::warn("Out of date KHR");
            } else if (res == VK_SUBOPTIMAL_KHR) {
                spdlog::warn("Not optimal swapchain");
            } else if (res != VK_SUCCESS) {
                spdlog::error("Failed to acquire swapchain image");
                core::fatal("Failed to acquire swapchain image");
            }
        }
        auto renderpass = std::static_pointer_cast<RenderPass_Vulkan>(swapchain_internal->renderpass.internal_state);

        commandList->renderpass = &swapchain_internal->renderpass;

        VkClearValue clearColor = {
                swapchain->desc.clearColor[0],
                swapchain->desc.clearColor[1],
                swapchain->desc.clearColor[2],
                swapchain->desc.clearColor[3],
        };

        VkRenderPassBeginInfo renderPassInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderpass->renderpass,
            .framebuffer = swapchain_internal->swapchainFramebuffers[swapchain_internal->currentImageIndex],
            .renderArea = {
                    .offset = {0, 0},
                    .extent = swapchain_internal->swapChainExtent,
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor,
        };
        auto cmdBuffer = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        vkCmdBeginRenderPass(cmdBuffer->commandBuffers[cmdBuffer->currentIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return true;
    }

    bool GraphicsDevice_Vulkan::endRenderPass(const Ref<graphics::CommandList>& cmd) {
        SP_PROFILE_FUNCTION();

        auto cmdBuffer = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        vkCmdEndRenderPass(cmdBuffer->commandBuffers[cmdBuffer->currentIndex]);
        return true;
    }

    bool GraphicsDevice_Vulkan::submitCommandList(const Ref<graphics::CommandList>& cmd) {
        SP_PROFILE_FUNCTION();

        auto internal_state = std::static_pointer_cast<CommandList_Vulkan>(cmd->internal_state);
        auto cmdBuffer = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        vkEndCommandBuffer(cmdBuffer->commandBuffers[cmdBuffer->currentIndex]);

        CommandQueue& queue = queues[m_queueFamilies.graphicsFamily.value()];
        queue.queue = m_graphicsQueue;
        queue.submit_cmds.push_back(cmdBuffer->commandBuffers[cmdBuffer->currentIndex]);
        auto swapchain_internal = std::static_pointer_cast<SwapChain_Vulkan>(internal_state->swapchain->internal_state);

        queue.submit_swapchains.push_back(swapchain_internal->swapchain);
        queue.submit_swapChainImageIndices.push_back(swapchain_internal->currentImageIndex);
        queue.submit_waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        queue.submit_waitSemaphores.push_back(swapchain_internal->acquire_semaphore);
        queue.submit_waitValues.push_back(0); // not a timeline semaphore
        queue.submit_signalSemaphores.push_back(swapchain_internal->release_semaphore);
        queue.submit_signalValues.push_back(0); // not a timeline semaphore

        queue.submit(this, VK_NULL_HANDLE); // TODO : potential issue with this queue

        return true;
    }

    bool GraphicsDevice_Vulkan::fastDraw(const Ref<SwapChain>& swapchain, const Ref<GraphicsPipeline>& pipeline, const Ref<CommandList>& cmd) {
        SP_PROFILE_FUNCTION();

        auto swapchain_internal = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);
        auto pipeline_internal  = std::static_pointer_cast<GraphicsPipeline_Vulkan>(pipeline->internal_state);
        auto cmd_internal       = std::static_pointer_cast<CommandBuffer_Vulkan>(cmd->commandBuffer->internal_state);

        vkCmdBindPipeline(cmd_internal->commandBuffers[cmd_internal->currentIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_internal->pipeline);

        VkViewport viewport {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(swapchain_internal->swapChainExtent.width),
            .height = static_cast<float>(swapchain_internal->swapChainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport(cmd_internal->commandBuffers[cmd_internal->currentIndex], 0, 1, &viewport);

        VkRect2D scissor {
            .offset = {0, 0},
            .extent = swapchain_internal->swapChainExtent,
        };
        vkCmdSetScissor(cmd_internal->commandBuffers[cmd_internal->currentIndex], 0, 1, &scissor);

        vkCmdDraw(cmd_internal->commandBuffers[cmd_internal->currentIndex], 3, 1, 0, 0);
        return true;
    }


    void GraphicsDevice_Vulkan::CommandQueue::submit(spring::graphics::GraphicsDevice_Vulkan *device, VkFence fence) {
        SP_PROFILE_FUNCTION();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = (uint32_t)submit_cmds.size();
        submitInfo.pCommandBuffers = submit_cmds.data();

        submitInfo.waitSemaphoreCount = (uint32_t)submit_waitSemaphores.size();
        submitInfo.pWaitSemaphores = submit_waitSemaphores.data();
        submitInfo.pWaitDstStageMask = submit_waitStages.data();

        submitInfo.signalSemaphoreCount = (uint32_t)submit_signalSemaphores.size();
        submitInfo.pSignalSemaphores = submit_signalSemaphores.data();

        VkTimelineSemaphoreSubmitInfo timelineInfo = {};
        timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.pNext = nullptr;
        timelineInfo.waitSemaphoreValueCount = (uint32_t)submit_waitValues.size();
        timelineInfo.pWaitSemaphoreValues = submit_waitValues.data();
        timelineInfo.signalSemaphoreValueCount = (uint32_t)submit_signalValues.size();
        timelineInfo.pSignalSemaphoreValues = submit_signalValues.data();

        submitInfo.pNext = &timelineInfo;

        VkResult res = vkQueueSubmit(queue, 1, &submitInfo, fence);
        assert(res == VK_SUCCESS);

        if (!submit_swapchains.empty())
        {
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = (uint32_t)submit_signalSemaphores.size();
            presentInfo.pWaitSemaphores = submit_signalSemaphores.data();
            presentInfo.swapchainCount = (uint32_t)submit_swapchains.size();
            presentInfo.pSwapchains = submit_swapchains.data();
            presentInfo.pImageIndices = submit_swapChainImageIndices.data();
            res = vkQueuePresentKHR(queue, &presentInfo);
        }

        swapchain_updates.clear();
        submit_swapchains.clear();
        submit_swapChainImageIndices.clear();
        submit_waitStages.clear();
        submit_waitSemaphores.clear();
        submit_waitValues.clear();
        submit_signalSemaphores.clear();
        submit_signalValues.clear();
        submit_cmds.clear();
    }
}
