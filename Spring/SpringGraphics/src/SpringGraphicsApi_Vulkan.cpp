//#include <Spring/SpringGraphics/SpringGraphicsApi_Vulkan.hpp>
#include "SpringGraphicsApi_Vulkan.hpp"

#include <Spring/SpringCore/SpringCore.hpp>

//#include <spdlog/spdlog.h>
#include <Spring/SpringGraphics/SpringWindow_Glfw.hpp>
#include <Spring/SpringGraphics/ISpringWindow.hpp>

namespace spring::graphics
{
    SpringGraphicsApi_Vulkan::SpringGraphicsApi_Vulkan() : /*m_surfaces{}, */m_devices{}
    {
        SP_PROFILE_FUNCTION();
        m_requiredExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	// Enable surface extensions depending on os
#if defined(SP_WIN32)
	    m_requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        m_requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        //m_requiredExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
        //m_requiredExtensions.push_back("VK_EXT_validation_features");
        m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
    }

	SpringGraphicsApi_Vulkan::~SpringGraphicsApi_Vulkan()
	{
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif
        m_devices.clear();
        m_surfaces.clear();
        //m_surfaces.clear();
        vkGetInstanceProcAddr(m_instance, "name");

        vkDestroyInstance(m_instance, nullptr);
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
        SP_PROFILE_FUNCTION();

        createInstance();
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        setupDebugMessenger();
#endif
	}

	void SpringGraphicsApi_Vulkan::shutdown()
	{
        
	}

	void SpringGraphicsApi_Vulkan::createInstance()
	{
        SP_PROFILE_FUNCTION();

#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif
        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "SpringApplication",
            .applicationVersion = VK_MAKE_VERSION(1,0,0),
            .pEngineName = "SpringEngine",
            .engineVersion = VK_MAKE_VERSION(1,0,0),
            .apiVersion = VK_API_VERSION_1_2
        };

        //m_requiredExtensions = getRequiredExtensions();

        //std::cout << "Extensions required by glfw" << std::endl;
        spdlog::info("Extensions required :");
        for (auto ext : m_requiredExtensions)
        {
            spdlog::info("  {}", ext);
        }

        uint32_t availableExtensionsCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());
        for (std::vector<const char*>::iterator reqExt = m_requiredExtensions.begin(); reqExt != m_requiredExtensions.end();)
        {
            bool found = false;
            for (const VkExtensionProperties& ext : availableExtensions)
            {
                if (strcmp(ext.extensionName, *reqExt)==0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                spdlog::warn("Missing extension {} ", *reqExt);
                reqExt = m_requiredExtensions.erase(reqExt);
                //reqExt++;
            }
            else
            {
                reqExt++;
            }
        }
        //std::cout << "All required extensions where found" << std::endl;

        VkInstanceCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .enabledExtensionCount = (uint32_t)m_requiredExtensions.size(),
            .ppEnabledExtensionNames = m_requiredExtensions.data(),
        };
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        std::vector<VkValidationFeatureEnableEXT> validFeaturesEnabled = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
        VkValidationFeaturesEXT validFeatures =
        {
            .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
            .enabledValidationFeatureCount = (uint32_t)validFeaturesEnabled.size(),
            .pEnabledValidationFeatures = validFeaturesEnabled.data()
        };
        createInfo.pNext = &validFeatures;
#endif

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif
        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
	}

    GraphicsSurface* SpringGraphicsApi_Vulkan::getSurface(SpringWindow* window)
    {
        Scope<GraphicsSurface> surface = makeScope<GraphicsSurface>();

        Ref<GraphicsSurface_Vulkan> surface_internal = makeRef<GraphicsSurface_Vulkan>();
#if defined(SP_WIN32)
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	    surfaceCreateInfo.hinstance = spring::core::SpringApplication::get()->getNativeInstance();
	    surfaceCreateInfo.hwnd = (HWND)window->getHandle();
	    VkResult err = vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &surface_internal->surface);

#elif defined(SP_LINUX)
        VkResult res = glfwCreateWindowSurface(m_instance, window->getHandle(), nullptr, &surface_internal->surface);
        if (res != VK_SUCCESS)
        {
            const char* description;
            int code = glfwGetError(&description);

            if (description)
                spdlog::error("code: {}, {}", code, description);
            spdlog::error("Error while creating window surface! ({})", res);
        }
#endif
        surface_internal->relatedInstance = m_instance;
        surface->internal_state = surface_internal;
        GraphicsSurface* surfacePtr = surface.get();
        registerSurface(surface);

        return surfacePtr;
    }

    GraphicsDevice* SpringGraphicsApi_Vulkan::createDevice(GraphicsDeviceDesc desc = {})
	{
        Scope<GraphicsDevice_Vulkan> device = makeScope<GraphicsDevice_Vulkan>(desc, this);
        GraphicsDevice_Vulkan* devicePtr = device.get();
        m_devices.emplace_back(std::move(device));
        return devicePtr;
	}

#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
    void SpringGraphicsApi_Vulkan::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = SpringGraphicsApi_Vulkan::debugCallback;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL SpringGraphicsApi_Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error("[VALID.LAY] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info("[VALID.LAY] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            spdlog::warn("[VALID.LAY] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            //spdlog::trace("[VALID.LAY] {}", pCallbackData->pMessage);
            break;
        default:
            //spdlog::trace("[VALID.LAY] {}", pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    void SpringGraphicsApi_Vulkan::setupDebugMessenger() {
        SP_PROFILE_FUNCTION();

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        auto res = CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger);
        if (res != VK_SUCCESS) {
            spdlog::error("{}", res);
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    bool SpringGraphicsApi_Vulkan::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_validationLayers) {
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
    }


    VkResult SpringGraphicsApi_Vulkan::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
	    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void SpringGraphicsApi_Vulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
	    const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
#endif
}