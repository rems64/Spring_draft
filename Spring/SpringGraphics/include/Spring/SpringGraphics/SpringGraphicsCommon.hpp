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

	enum class Format
	{

	};


	enum QueueTypes
	{
		Graphics,
		Compute,
		Transfert,
		Protected,
		Count
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

	struct RenderPassDesc
	{
		enum class Flags
		{
			EMPTY = 0,
			ALLOW_UAV_WRITES = 1 << 0,
		};
		Flags flags = Flags::EMPTY;
		//std::vector<RenderPassAttachment> attachments;
	};

	struct RenderPass : public GraphicsDeviceChild
	{
		size_t hash = 0;
		RenderPassDesc desc;

		constexpr const RenderPassDesc& GetDesc() const { return desc; }
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

	enum class PrimitiveTopology
	{
		Triangles,
		LineList,
		LineStrip,
		Point
	};

	enum class FillMode
	{
		Fill,
		Wireframe
	};

	enum class CullMode
	{
		None,
		Front,
		Back
	};

	struct RasterizerDesc
	{
		FillMode fill = FillMode::Fill;
		CullMode cullmode = CullMode::None;
		bool frontClockwise = true;
		bool depthClampEnable = false;
		bool discard = false;
		float depthBias = 0.0f;
		float depthBiasClamp = 0.0f;
		float depthBiasSlopeFactor = 0.0f;
	};

	struct GraphicsPipelineDesc
	{
		PrimitiveTopology topology;
		RasterizerDesc rasterizer;
	};

	struct GaphicsPipeline : public GraphicsDeviceChild
	{
		GraphicsPipelineDesc desc;

		inline const GraphicsPipelineDesc& getDesc() const { return desc; };
	};
}