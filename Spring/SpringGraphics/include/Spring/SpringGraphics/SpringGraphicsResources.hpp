#pragma once

namespace spring::graphics
{
	enum class ShaderStages // Do not forget to update the shader stages count constant below
	{
		None,
		VertexShader,
		FragmentShader
	};

	static constexpr const size_t ShaderStagesCount = 2;

	enum class Format
	{
		SRGB_8,
		B8G8R8A8_SRGB,
		B8G8R8A8_UNORM,
		R8G8B8A8_SRGB
	};

	struct GraphicsResource
	{
		std::shared_ptr<void> data;
		inline bool isValid() const { return data.get() != nullptr; };
	};

	struct SwapChainDesc
	{
		uint32_t width;
		uint32_t height;
		Format format;
		bool fullscreen;
		bool vsync;
	};

	struct SwapChain : public GraphicsResource
	{
		SwapChainDesc desc;

		constexpr const SwapChainDesc& getDesc() { return desc; };
	};

	struct Shader : public GraphicsResource
	{
		ShaderStages shaderStage = ShaderStages::None;
	};

	struct InputElement
	{
		size_t size;
	};

	struct InputDescription
	{
		InputElement* elements;
	};

	struct PipelineStateDesc
	{
		const Shader* vertexShader;
		const Shader* fragmentShader;
		const InputDescription* inputDesc = nullptr;
	};

	struct PipelineState : public GraphicsResource
	{
		PipelineStateDesc desc;

		constexpr const PipelineStateDesc& getDesc() { return desc; };
	};

	enum class RenderPassAttachmentTypes
	{
		None,
		RenderTarget
	};

	enum class RenderPassLoadOp
	{
		Load,
		Clear,
		DontCare
	};

	struct TextureDesc
	{
		Format format;
	};

	struct Texture : public GraphicsResource
	{
		TextureDesc desc;

		constexpr const TextureDesc& getDesc() { return desc; };
	};

	enum class RenderPassStoreOp
	{
		Store,
		DontCare
	};

	struct RenderPassAttachment
	{
		RenderPassAttachmentTypes type;
		RenderPassLoadOp loadOp;
		RenderPassStoreOp storeOp;

		const Texture* texture;
	};

	struct RenderPassDesc
	{
		std::vector<RenderPassAttachment> attachments;
	};

	struct RenderPass : public GraphicsResource
	{
		RenderPassDesc desc;
		constexpr const RenderPassDesc& getDesc() { return desc; };
	};

	struct CommandList : public GraphicsResource
	{
	};
}