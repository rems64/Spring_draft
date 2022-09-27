#pragma once
#ifndef NDEBUG
//#define SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>
#include "SpringGraphicsDevice_Vulkan.hpp"

namespace spring::graphics
{
	struct GraphicsSurface_Vulkan
	{
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkInstance relatedInstance;

		~GraphicsSurface_Vulkan()
		{
			vkDestroySurfaceKHR(relatedInstance, surface, nullptr);
		}
	};

	class SpringGraphicsApi_Vulkan : public SpringGraphicsApi
	{
		friend GraphicsDevice_Vulkan; // Mainly for compatibility with validation layers needed to be enabled in device prior v1.3
	public:
		SpringGraphicsApi_Vulkan();
		~SpringGraphicsApi_Vulkan();

		const char* getName();
		GraphicsApiProperties getProperties();

		virtual void init();
		virtual void shutdown();

		//virtual GraphicsSurface* getSurface(SpringWindow* window) override;
		void createInstance();
		inline const VkInstance* getInstance() { return &m_instance; };
		virtual GraphicsDevice* createDevice(GraphicsDeviceDesc desc) override;
		//void registerSurface(Scope<GraphicsSurface>& surface) { m_surfaces.emplace_back(std::move(surface)); };
	private:
		VkInstance m_instance;
		std::vector<Scope<GraphicsDevice_Vulkan>> m_devices;
		//std::vector<Scope<GraphicsSurface>> m_surfaces;
		std::vector<const char*> m_requiredExtensions;
#ifdef SPRING_VULKAN_ENABLE_VALIDATION_LAYERS
		void setupDebugMessenger();
		bool checkValidationLayerSupport();
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		std::vector<const char*> m_validationLayers;
		VkDebugUtilsMessengerEXT m_debugMessenger;
#endif
	};

}