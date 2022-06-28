#pragma once
#define SPRING_VULKAN_ENABLE_VALIDATION_LAYERS

#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>
#include <Spring/SpringGraphics/SpringGraphicsDevice_Vulkan.hpp>

namespace spring::graphics
{
	class SpringGraphicsApi_Vulkan : public SpringGraphicsApi
	{
	public:
		SpringGraphicsApi_Vulkan();
		~SpringGraphicsApi_Vulkan();

		const char* getName();
		GraphicsApiProperties getProperties();

		virtual void init();
		virtual void shutdown();

		void createInstance();
		inline const VkInstance* getInstance() { return &m_instance; };
		virtual GraphicsDevice* createDevice(GraphicsDeviceDesc desc) override;
	private:
		VkInstance m_instance;
		std::vector<Scope<GraphicsDevice_Vulkan>> m_devices;

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