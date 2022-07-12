#pragma once

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::graphics
{
	// Structure from Wicked engine : https://github.com/turanszkij/WickedEngine
	struct GraphicsDeviceChild
	{
		Ref<void> internal_state;
		[[nodiscard]] bool isValid() const { return internal_state.get() != nullptr; }
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

	enum class ShaderStage
	{
		Vertex,
		Fragment,
		None
	};

	struct ShaderDesc
	{
		ShaderStage stage;
		const void* source;
		size_t size;
	};

	struct Shader : public GraphicsDeviceChild
	{
		ShaderStage stage = ShaderStage::None;
	};

	struct GraphicsSurface : public GraphicsDeviceChild
	{
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct RenderPassDesc
	{
		enum class Flags
		{
			Empty = 0,
			Allow_Uav_Writes = 1 << 0,
		};
		Flags flags = Flags::Empty;
		//std::vector<RenderPassAttachment> attachments;
	};

	struct RenderPass : public GraphicsDeviceChild
	{
		size_t hash = 0;
		RenderPassDesc desc;

		[[nodiscard]] constexpr const RenderPassDesc& GetDesc() const { return desc; }
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

		[[nodiscard]] const SwapChainDesc& getDesc() const { return desc; }
	};

	enum class PrimitiveTopology
	{
		Triangles,
		LineList,
		LineStrip,
		Point,
		None
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
		PrimitiveTopology topology = PrimitiveTopology::None;
		RasterizerDesc rasterizer = {};
	};

	struct GaphicsPipeline : public GraphicsDeviceChild
	{
		GraphicsPipelineDesc desc;

		[[nodiscard]] const GraphicsPipelineDesc& getDesc() const { return desc; }
	};
}