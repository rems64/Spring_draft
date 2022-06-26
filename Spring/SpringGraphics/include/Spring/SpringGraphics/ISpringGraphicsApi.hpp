#pragma once

namespace spring::graphics
{
	struct GraphicsApiProperties
	{
		const char* name;
		const char* version;
	};


	class SpringGraphicsApi
	{
	public:
		SpringGraphicsApi() {};
		virtual ~SpringGraphicsApi() {};

		virtual const char* getName() = 0;
		virtual GraphicsApiProperties getProperties() = 0;

		virtual void init() = 0;
		virtual void shutdown() = 0;

		static SpringGraphicsApi* build();
	};
}