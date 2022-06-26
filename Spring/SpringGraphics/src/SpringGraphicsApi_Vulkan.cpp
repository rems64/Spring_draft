#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>

#include <Spring/SpringCore/SpringCore.hpp>

#include <spdlog/spdlog.h>

namespace spring::graphics
{
    SpringGraphicsApi_Vulkan::SpringGraphicsApi_Vulkan()
    {
#ifdef GLFW3
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            m_requiredExtensions.push_back(glfwExtensions[i]);
        }
#endif

#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        m_requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    }

	SpringGraphicsApi_Vulkan::~SpringGraphicsApi_Vulkan()
	{

	}

	const char* SpringGraphicsApi_Vulkan::getName()
	{
		return "VulkanAPI";
	}

    GraphicsApiProperties SpringGraphicsApi_Vulkan::getProperties()
    {
        return {
            .name = "name",
            .version = "1.0"
        };
    }

	void SpringGraphicsApi_Vulkan::init()
	{
		
	}

	void SpringGraphicsApi_Vulkan::shutdown()
	{

	}

	void SpringGraphicsApi_Vulkan::createInstance()
	{
        /*
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif
        vk::ApplicationInfo appInfo = {
            .pApplicationName = "SpringApplication",
            .applicationVersion = VK_MAKE_VERSION(1,0,0),
            .pEngineName = "No engine",
            .engineVersion = VK_MAKE_VERSION(1,0,0),
            .apiVersion = VK_API_VERSION_1_3
        };

        //m_requiredExtensions = getRequiredExtensions();

        //std::cout << "Extensions required by glfw" << std::endl;
        spdlog::info("Extensions required by glfw");
        for (auto ext : requiredExtensions)
        {
            spdlog::info("{}", ext);
        }

        uint32_t availableExtensionsCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());
        for (auto reqExt : requiredExtensions)
        {
            bool found = false;
            for (auto ext : availableExtensions)
            {
                if (strcmp(ext.extensionName, reqExt))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                spdlog::info("Missing extension {} which is required by glfw", reqExt);
                ;                return;
            }
        }
        //std::cout << "All required extensions where found" << std::endl;
        spdlog::info("All required extensions where found");

        VkInstanceCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .enabledExtensionCount = (uint32_t)requiredExtensions.size(),
            .ppEnabledExtensionNames = requiredExtensions.data()
        };

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
	}

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        auto res = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
        if (res != VK_SUCCESS) {
            spdlog::error("{}", res);
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
        */
    }

	void SpringGraphicsApi_Vulkan::setupDebugMessenger()
	{
	}

	void SpringGraphicsApi_Vulkan::pickPhysicalDevice()
	{
	}

	void SpringGraphicsApi_Vulkan::createLogicalDevice()
	{
	}
}