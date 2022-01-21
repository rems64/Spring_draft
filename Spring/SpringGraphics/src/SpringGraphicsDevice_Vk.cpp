#include <Spring/SpringGraphics/SpringGraphicsDevice_Vk.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>
#include <Spring/SpringGraphics/SpringWindow.hpp>
//#include <Spring/SpringGraphics/SpringWindow_Native.hpp>
#include <Spring/SpringCore/SpringApplication.hpp>

namespace spring::graphics
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
	{
        std::cout << "[Vulkan Warning]: " << callback_data->pMessage << std::endl;

		return VK_FALSE;
	}

    Device_Vulkan::Device_Vulkan()
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
        instanceExtensions.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
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

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
            spring::core::error("Failed to create logical device!");
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_device, queueIndex, 0, &m_graphicsQueue);
    }

    Device_Vulkan::~Device_Vulkan()
    {
        vkDestroyDevice(m_device, nullptr);
        ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"))(m_instance, m_debugUtilsMessenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    int Device_Vulkan::createSwapChain(SwapChain* swapchain, SwapChainDesc swapchainDesc, spring::graphics::SpringWindow* window)
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window->getHandle();
        createInfo.hinstance = spring::core::SpringApplication::get()->getNativeInstance();

        if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
}