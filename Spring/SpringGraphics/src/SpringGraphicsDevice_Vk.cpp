#include <Spring/SpringGraphics/SpringGraphicsDevice_Vk.hpp>
#include "SpringGraphicsDevice_Vk_private.hpp"

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringGraphics/SpringWindow.hpp>
//#include <Spring/SpringGraphics/SpringWindow_Native.hpp>
#include <Spring/SpringCore/SpringApplication.hpp>

namespace spring::graphics
{
    VkFormat convertFormat(Format format)
    {
        switch (format)
        {
        case spring::graphics::Format::SRGB_8:
            return VK_FORMAT_R8G8B8_SRGB;
        case spring::graphics::Format::SRGBA_8:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case spring::graphics::Format::RGBA_16:
            return VK_FORMAT_R8G8B8A8_SSCALED;
        default:
            break;
        }
    }

    VkAttachmentLoadOp convertLoadOp(RenderPassLoadOp loadOp)
    {
        switch (loadOp)
        {
        case spring::graphics::RenderPassLoadOp::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case spring::graphics::RenderPassLoadOp::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case spring::graphics::RenderPassLoadOp::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    VkAttachmentStoreOp convertStoreOp(RenderPassStoreOp storeOp)
    {
        switch (storeOp)
        {
        case spring::graphics::RenderPassStoreOp::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case spring::graphics::RenderPassStoreOp::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_STORE_OP_STORE;
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
	{
        std::cout << "[Vulkan Warning]: " << callback_data->pMessage << std::endl;

		return VK_FALSE;
	}

    Device_Vulkan::Device_Vulkan() : m_allocationCollector(std::make_shared<AllocationCollector>())
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Spring Engine app";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Spring Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;
        
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::vector<const char*> instanceExtensions;

    #if defined(VK_USE_PLATFORM_WIN32_KHR)
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        //instanceExtensions.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
        //instanceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    #endif
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

         //for(auto extension : extensions)
         //{
         //    std::cout << extension.extensionName << std::endl;
         //}


        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();


        // Validation layers, should be improved by checking for availability
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // for(auto layer : availableLayers)
        // {
        //     std::cout << layer.layerName << std::endl;
        // }

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // Here checking for debug mode would be nice
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };

        debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;
        createInfo.pNext = &debugUtilsCreateInfo;

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            spring::core::error("Failed to create Vulkan instance");
            throw std::runtime_error("failed to create instance!");
        }

        ((PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"))(m_instance, &debugUtilsCreateInfo, nullptr, &m_debugUtilsMessenger);

        // Get physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        if (deviceCount == 0) {
            spring::core::error("Failed to find GPUs with Vulkan support!");
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::cout << "Available devices (" << deviceCount << "):" << std::endl;
        for (const auto& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            std::cout << "  Device : " << deviceProperties.deviceName << std::endl;
        }

        // Need to be written again to get the best device
        m_physicalDevice = devices[0];

        // Get queue family properties
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

        std::cout << "Available queue families (" << queueFamilyCount << "):" << std::endl;
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            std::cout << "  Queue family " << i << ": " << std::endl;
            std::cout << "    Graphics queue: " << ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "yes" : "no") << std::endl;
            std::cout << "    Compute queue: " << ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) ? "yes" : "no") << std::endl;
            std::cout << "    Transfer queue: " << ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) ? "yes" : "no") << std::endl;
            std::cout << "    Queue count: " << queueFamilies[i].queueCount << std::endl;
            std::cout << "    Timestamp valid bits: " << queueFamilies[i].timestampValidBits << std::endl;
            std::cout << "    Min image transfer granularity: " << queueFamilies[i].minImageTransferGranularity.width << "x" << queueFamilies[i].minImageTransferGranularity.height << "x" << queueFamilies[i].minImageTransferGranularity.depth << std::endl;
        }

        // Select first matching queue
        int queueIndex = 0;
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueIndex = i;
                break;
            }
        }

        // Create logical device
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        uint32_t deviceExtensionsCount;
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(deviceExtensionsCount);
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, availableExtensions.data());
        
        //std::cout << "Available extensions :" << std::endl;
        //for (VkExtensionProperties& extension : availableExtensions)
        //{
        //    std::cout << "  [extension] " << extension.extensionName << std::endl;
        //}

        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
            spring::core::error("Failed to create logical device!");
            throw std::runtime_error("failed to create logical device!");
        }

        m_allocationCollector->device = m_device;
        m_allocationCollector->instance = m_instance;

        vkGetDeviceQueue(m_device, queueIndex, 0, &m_graphicsQueue);
    }

    Device_Vulkan::~Device_Vulkan()
    {
        m_allocationCollector->collect();
        vkDestroyDevice(m_device, nullptr);
        ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"))(m_instance, m_debugUtilsMessenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    int Device_Vulkan::createSwapChain(SwapChain* swapchain, SwapChainDesc& swapchainDesc, spring::graphics::SpringWindow* window)
    {
        std::cout << "Creating swapchain" << std::endl;
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window->getHandle();
        createInfo.hinstance = spring::core::SpringApplication::get()->getNativeInstance();

        std::shared_ptr<SwapChain_Vk> data = std::make_shared<SwapChain_Vk>();
        data->allocationCollector = m_allocationCollector;
        swapchain->data = data;
        swapchain->desc = swapchainDesc;

        if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &data->surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, data->surface, &capabilities);

        std::vector<VkSurfaceFormatKHR> availableFormats = {};
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, data->surface, &formatCount, nullptr);
        availableFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, data->surface, &formatCount, availableFormats.data());

        VkSurfaceFormatKHR surfaceFormat;
        surfaceFormat = availableFormats[0];
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = availableFormat;
            }
        }

        std::vector<VkPresentModeKHR> availablePresentModes = {};
        uint32_t presentModesCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, data->surface, &presentModesCount, nullptr);
        availablePresentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, data->surface, &presentModesCount, availablePresentModes.data());

        VkPresentModeKHR presentMode;
        presentMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = availablePresentMode;
            }
        }

        VkExtent2D extent;

        if (capabilities.currentExtent.width != UINT32_MAX) {
            extent = capabilities.currentExtent;
        }
        else {
            int width = window->getWidth();
            int height = window->getHeight();

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            extent = actualExtent;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = data->surface;
        swapchainCreateInfo.minImageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        // TODO --> handle different queue for presentation and graphics
        swapchainCreateInfo.preTransform = capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &data->swapchain))
        {
            spring::core::error("Failed to create swapchain");
            throw std::runtime_error("Failed to create swapchain");
        }

        uint32_t swapChainImagesCount;
        vkGetSwapchainImagesKHR(m_device, data->swapchain, &swapChainImagesCount, nullptr);
        data->swapChainImages.resize(swapChainImagesCount);
        vkGetSwapchainImagesKHR(m_device, data->swapchain, &swapChainImagesCount, data->swapChainImages.data());

        data->extent = extent;
        data->imageFormat = surfaceFormat.format;

        data->swapChainImageViews.resize(swapChainImagesCount);

        for (size_t i = 0; i < data->swapChainImages.size(); i++) {
            VkImageViewCreateInfo imViewCreateInfo{};
            imViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imViewCreateInfo.image = data->swapChainImages[i];
            imViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imViewCreateInfo.format = data->imageFormat;
            imViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imViewCreateInfo.subresourceRange.levelCount = 1;
            imViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imViewCreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device, &imViewCreateInfo, nullptr, &data->swapChainImageViews[i]) != VK_SUCCESS) {
                spring::core::error("Failed creating image views");
                throw std::runtime_error("failed to create image views!");
            }
        }

        return 0;
    }

    int Device_Vulkan::createShader(Shader* shader, ShaderStages shaderStage, const void* source, size_t size)
    {
        std::shared_ptr<Shader_Vk> vkShader = std::make_shared<Shader_Vk>();
        vkShader->allocationCollector = m_allocationCollector;
        shader->data = vkShader;
        shader->shaderStage = shaderStage;

        VkShaderModuleCreateInfo smCreateInfo = {};
        smCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        smCreateInfo.codeSize = size;
        smCreateInfo.pCode = reinterpret_cast<const uint32_t*>(source);
        if (vkCreateShaderModule(m_device, &smCreateInfo, nullptr, &vkShader->shaderModule))
        {
            spring::core::error("Failed to create shader module");
            throw std::runtime_error("Failed to create shader module");
        }

        vkShader->stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkShader->stageInfo.module = vkShader->shaderModule;
        vkShader->stageInfo.pName = "main";
        switch (shaderStage)
        {
        case ShaderStages::VertexShader:
            vkShader->stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderStages::FragmentShader:
            vkShader->stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            vkShader->stageInfo.stage = VK_SHADER_STAGE_ALL;
            break;
        }

        return 0;
    }

    int Device_Vulkan::createPipelineState(PipelineState* pipelineState, PipelineStateDesc* desc)
    {
        std::shared_ptr<PipelineState_Vk> psVk = std::make_shared<PipelineState_Vk>();
        psVk->allocationCollector = m_allocationCollector;
        pipelineState->desc = *desc;
        pipelineState->data = psVk;

        psVk->vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        psVk->vertexInputInfo.vertexBindingDescriptionCount = 0;
        psVk->vertexInputInfo.pVertexBindingDescriptions = nullptr;
        psVk->vertexInputInfo.vertexAttributeDescriptionCount = 0;
        psVk->vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        psVk->inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        psVk->inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        psVk->inputAssembly.primitiveRestartEnable = VK_FALSE;

        psVk->viewport.x = 0.0f;
        psVk->viewport.y = 0.0f;
        psVk->viewport.width = 65535;
        psVk->viewport.height = 65535;
        psVk->viewport.minDepth = 0.0f;
        psVk->viewport.maxDepth = 1.0f;

        psVk->scissor.offset = { 0, 0 };
        psVk->scissor.extent.width = 65535;
        psVk->scissor.extent.height = 65535;

        psVk->viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        psVk->viewportState.viewportCount = 1;
        psVk->viewportState.pViewports = &psVk->viewport;
        psVk->viewportState.scissorCount = 1;
        psVk->viewportState.pScissors = &psVk->scissor;

        psVk->rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        psVk->rasterizer.depthClampEnable = VK_FALSE;
        psVk->rasterizer.rasterizerDiscardEnable = VK_FALSE;
        psVk->rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        psVk->rasterizer.lineWidth = 1.0f;
        psVk->rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        psVk->rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        psVk->rasterizer.depthBiasEnable = VK_FALSE;
        psVk->rasterizer.depthBiasConstantFactor = 0.0f;
        psVk->rasterizer.depthBiasClamp = 0.0f;
        psVk->rasterizer.depthBiasSlopeFactor = 0.0f;

        psVk->colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        psVk->colorBlendAttachments[0].blendEnable = VK_FALSE;
        psVk->colorBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        psVk->colorBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        psVk->colorBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD; // Optional
        psVk->colorBlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        psVk->colorBlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        psVk->colorBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        psVk->colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        psVk->colorBlending.logicOpEnable = VK_FALSE;
        psVk->colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        psVk->colorBlending.attachmentCount = static_cast<uint32_t>(psVk->colorBlendAttachments.size());
        psVk->colorBlending.pAttachments = psVk->colorBlendAttachments.data();
        psVk->colorBlending.blendConstants[0] = 0.0f; // Optional
        psVk->colorBlending.blendConstants[1] = 0.0f; // Optional
        psVk->colorBlending.blendConstants[2] = 0.0f; // Optional
        psVk->colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &psVk->pipelineLayout) != VK_SUCCESS) {
            spring::core::error("Failed to create pipeline layout!");
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkPipelineShaderStageCreateInfo* shadStag = psVk->shaderStages;
        uint32_t shadStagIdx = 0;
        if (desc->fragmentShader && desc->fragmentShader->isValid())
        {
            shadStag[shadStagIdx] = std::static_pointer_cast<Shader_Vk>(desc->fragmentShader->data)->stageInfo;
            shadStagIdx++;
        }

        psVk->pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        psVk->pipelineCreateInfo.stageCount = shadStagIdx;
        psVk->pipelineCreateInfo.pStages = shadStag;
        psVk->pipelineCreateInfo.pVertexInputState = &psVk->vertexInputInfo;
        psVk->pipelineCreateInfo.pInputAssemblyState = &psVk->inputAssembly;
        psVk->pipelineCreateInfo.pViewportState = &psVk->viewportState;
        psVk->pipelineCreateInfo.pRasterizationState = &psVk->rasterizer;
        psVk->pipelineCreateInfo.pMultisampleState = nullptr;
        psVk->pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
        psVk->pipelineCreateInfo.pColorBlendState = &psVk->colorBlending;
        psVk->pipelineCreateInfo.pDynamicState = nullptr; // Optional
        psVk->pipelineCreateInfo.layout = psVk->pipelineLayout;
        //psVk->pipelineCreateInfo.renderPass = ;
        psVk->pipelineCreateInfo.subpass = 0;
        psVk->pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        psVk->pipelineCreateInfo.basePipelineIndex = -1; // Optional

        return 0;
    }

    int Device_Vulkan::createRenderPass(RenderPass* renderPass, RenderPassDesc* desc)
    {
        std::shared_ptr<RenderPass_Vk> rpVk = std::make_shared<RenderPass_Vk>();
        rpVk->allocationCollector = m_allocationCollector;
        renderPass->data = rpVk;
        renderPass->desc = *desc;

        std::vector<VkAttachmentReference> colorAttachmentRefs;
        std::vector<VkAttachmentDescription> attachmentDescriptions;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 0;
        subpass.pColorAttachments = colorAttachmentRefs.data();

        for (auto attachment : desc->attachments)
        {
            VkAttachmentDescription tmpAttachment{};
            tmpAttachment.format = convertFormat(attachment.texture->desc.format);
            tmpAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            tmpAttachment.loadOp = convertLoadOp(attachment.loadOp);
            tmpAttachment.storeOp = convertStoreOp(attachment.storeOp);
            tmpAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            tmpAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            tmpAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            tmpAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            if (attachment.type == RenderPassAttachmentTypes::RenderTarget)
            {
                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                colorAttachmentRefs.push_back(colorAttachmentRef);

                subpass.colorAttachmentCount++;
                subpass.pColorAttachments = colorAttachmentRefs.data();
            }
            attachmentDescriptions.push_back(tmpAttachment);
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &rpVk->renderPass) != VK_SUCCESS) {
            spring::core::error("Failed to create render pass");
            throw std::runtime_error("Failed to create render pass!");
        }

        return 0;
    }

    int Device_Vulkan::validatePipeline()
    {

        if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfos, nullptr, &pipeline))
        {
            spring::core::error("Failed to create pipeline");
            throw std::runtime_error("Failed to create pipeline");
        }
        return 0;
    }
}