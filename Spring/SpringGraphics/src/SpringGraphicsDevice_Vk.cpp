#include <Spring/SpringGraphics/SpringGraphicsDevice_Vk.hpp>

#include <Spring/SpringCore/SpringMisc.hpp>
#include <vector>
#include <iostream>

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
    #endif
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // for(auto extension : extensions)
        // {
        //     std::cout << extension.extensionName << std::endl;
        // }


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
        // func(m_instance, &debugUtilsCreateInfo, nullptr, &m_debugUtilsMessenger);
    }

    Device_Vulkan::~Device_Vulkan()
    {
        ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"))(m_instance, m_debugUtilsMessenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
}