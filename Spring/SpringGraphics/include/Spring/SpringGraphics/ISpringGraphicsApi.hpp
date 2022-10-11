#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::graphics
{
	class GraphicsDevice;
	struct GraphicsDeviceDesc;
	struct GraphicsSurface;
	class SpringWindow;
	struct GraphicsApiProperties
	{
		const char* name;
		const char* version;
	};


	class SpringGraphicsApi
	{
	public:
		SpringGraphicsApi() = default;
		virtual ~SpringGraphicsApi() = default;

		virtual const char* getName() = 0;
		virtual GraphicsApiProperties getProperties() = 0;

		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual GraphicsSurface* getSurface(SpringWindow* window) = 0;

		virtual GraphicsDevice* createDevice(GraphicsDeviceDesc desc) = 0;

		static Scope<SpringGraphicsApi> build();
	};
}