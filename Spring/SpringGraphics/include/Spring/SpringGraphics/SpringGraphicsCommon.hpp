#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::graphics
{
	// Structure from Wicked engine : https://github.com/turanszkij/WickedEngine
	struct GraphicsDeviceChild
	{
		Ref<void> internal_state;
		inline bool isValid() { return internal_state.get() != nullptr; };
	};


	enum class QueueTypes
	{
		Graphics	= 1<<0,
		Compute		= 1<<1,
		Transfert	= 1<<2,
		Protected	= 1<<3
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
	};


	struct GraphicsSurface : public GraphicsDeviceChild
	{
		uint32_t width;
		uint32_t height;
	};

	struct SwapChainDesc
	{
		uint32_t width = 640;
		uint32_t height = 480;
		bool hasSurface = true;
		GraphicsSurface* surface = nullptr;
	};

	struct SwapChain : public GraphicsDeviceChild
	{
		SwapChainDesc desc;

		inline const SwapChainDesc& getDesc() const { return desc; };
	};

#ifdef SPRING_BUILD_VK
	struct GraphicsSurface_Vulkan
	{
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkInstance relatedInstance;

		~GraphicsSurface_Vulkan()
		{
			vkDestroySurfaceKHR(relatedInstance, surface, nullptr);
		}
	};
#endif
}