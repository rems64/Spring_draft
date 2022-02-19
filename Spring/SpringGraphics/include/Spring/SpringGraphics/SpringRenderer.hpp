#pragma once

#include "SpringGraphicsDevice.hpp"

namespace spring::graphics
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

	protected:
		RenderPass renderPass;
	};
}