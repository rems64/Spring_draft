#include <Spring/SpringGraphics/ISpringGraphicsApi.hpp>

#define SPRING_VULKAN_ENABLE_VALIDATION_LAYERS

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
		void setupDebugMessenger();
		void pickPhysicalDevice();
		void createLogicalDevice();
	private:
		vk::Instance m_instance;

		std::vector<const char*> m_requiredExtensions;
	};
}