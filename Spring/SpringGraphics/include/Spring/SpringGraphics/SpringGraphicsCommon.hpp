#pragma once

#include <Spring/SpringCore/SpringCore.hpp>

namespace spring::graphics {
    class SpringWindow;

    // Structure from Wicked engine : https://github.com/turanszkij/WickedEngine
    struct GraphicsDeviceChild {
        Ref<void> internal_state;

        [[nodiscard]] bool isValid() const { return internal_state.get() != nullptr; }
    };

    enum class Format {
        //RG
        RG_32_SFLOAT,
        RG_32_SINT,
        RG_32_UINT,

        //RGB
        RGB_32_SFLOAT,
        RGB_32_SINT,
        RGB_32_UINT,

        //RGBA
        RGBA_32_SFLOAT,
        RGBA_32_SINT,
        RGBA_32_UINT,

        Empty,
    };


    enum QueueTypes {
        Graphics,
        Compute,
        Transfert,
        Protected,
        Count
    };

    enum class ResourceState
    {
        // Common resource states:
        Undefined                   = 0,        // invalid state (don't preserve contents)
        ShaderResource              = 1 << 0,   // shader resource, read only
        ShaderResourceCompute       = 1 << 1,   // shader resource, read only, non-pixel shader
        UnorderedAccess             = 1 << 2,   // shader resource, write enabled
        CopySrc                     = 1 << 3,   // copy from
        CopyDst                     = 1 << 4,   // copy to

        // Texture specific resource states:
        RenderTarget                = 1 << 5,   // render target, write enabled
        DepthStencil                = 1 << 6,   // depth stencil, write enabled
        DepthStencilReadOnly        = 1 << 7,   // depth stencil, read only
        ShadingRateSource           = 1 << 8,   // shading rate control per tile

        // GPUBuffer specific resource states:
        VertexBuffer                = 1 << 9,   // vertex buffer, read only
        IndexBuffer                 = 1 << 10,  // index buffer, read only
        ConstantBuffer              = 1 << 11,  // constant buffer, read only
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };

    enum class ShaderStage {
        Vertex,
        Fragment,
        None
    };

    struct ShaderDesc {
        ShaderStage stage;
        const void *source;
        size_t size;
    };

    struct Shader : public GraphicsDeviceChild {
        ShaderStage stage = ShaderStage::None;
    };

    struct GraphicsSurface : public GraphicsDeviceChild {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct BufferDesc
    {
        size_t size = 0;
    };

    struct Buffer : public GraphicsDeviceChild
    {
        BufferDesc desc = {};
    };

    struct TextureDesc
    {
        Format format = Format::Empty;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t samples = 0x08;
        uint32_t mipLevels = 1;
    };

    struct Texture : public GraphicsDeviceChild
    {
        TextureDesc desc;
    };

    struct RenderPassAttachment
    {
        enum class Type
        {
            RenderTarget,
            ColorResolve,
            DepthResolve,
            DepthStencil
        } type = Type::RenderTarget;

        Texture* texture = nullptr;

        enum class LoadOp
        {
            Load,
            Clear,
            DontCare,
        } loadOp = LoadOp::Clear;

        enum class StoreOp
        {
            Store,
            DontCare
        } storeOp = StoreOp::DontCare;

        ResourceState initialLayout = ResourceState::Undefined;
        ResourceState subpassLayout = ResourceState::Undefined;
        ResourceState finalLayout   = ResourceState::Undefined;

        static RenderPassAttachment RenderTarget(Texture* texture,
                                          LoadOp loadOp = LoadOp::Load,
                                          StoreOp storeOp = StoreOp::Store,
                                          ResourceState initialLayout = ResourceState::ShaderResource,
                                          ResourceState subpassLayout = ResourceState::RenderTarget,
                                          ResourceState finalLayout   = ResourceState::ShaderResource) {
            return {
                .type = Type::RenderTarget,
                .texture = texture,
                .loadOp = loadOp,
                .storeOp = storeOp,
                .initialLayout = initialLayout,
                .subpassLayout = subpassLayout,
                .finalLayout = finalLayout
            };
        }
    };

    struct RenderPassDesc {
        std::vector<RenderPassAttachment> attachments;
    };

    struct RenderPass : public GraphicsDeviceChild {
        size_t hash = 0;
        RenderPassDesc desc;

        [[nodiscard]] constexpr const RenderPassDesc &GetDesc() const { return desc; }
    };

    struct SwapChainDesc {
        uint32_t width = 640;
        uint32_t height = 480;
        std::array<float, 4> clearColor = {0.12f, 0.12f, 0.12f, 1.0f};
        bool presentWindow = true;
        SpringWindow *window = nullptr;
    };

    struct SwapChain : public GraphicsDeviceChild {
        SwapChainDesc desc;

        [[nodiscard]] const SwapChainDesc &getDesc() const { return desc; }
    };

    enum class PrimitiveTopology {
        Triangles,
        LineList,
        LineStrip,
        Point,
        None
    };

    enum class FillMode {
        Fill,
        Wireframe
    };

    enum class CullMode {
        None,
        Front,
        Back
    };

    struct RasterizerDesc {
        FillMode fill = FillMode::Fill;
        CullMode cullmode = CullMode::None;
        bool frontClockwise = true;
        bool depthClampEnable = false;
        bool discard = false;
        float depthBias = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
    };

    enum class InputRates {
        Vertex,
        Instance
    };

    struct InputLayout {
        struct VertexInput {
            Format format = Format::Empty;
            InputRates inputType = InputRates::Vertex;
            uint32_t binding = 0;
            uint32_t location = 0;
            uint32_t attributeOffset = ~0U;
        };
        std::vector<VertexInput> inputs;

    };

    struct GraphicsPipelineDesc {
        InputLayout inputLayout = {};
        Shader *vs = nullptr;
        Shader *fs = nullptr;
        PrimitiveTopology topology = PrimitiveTopology::None;
        RasterizerDesc rasterizer = {};
    };

    struct GraphicsPipeline : public GraphicsDeviceChild {
        GraphicsPipelineDesc desc;

        [[nodiscard]] const GraphicsPipelineDesc &getDesc() const { return desc; }
    };

    struct CommandBuffer : public GraphicsDeviceChild {
        QueueTypes queueType = QueueTypes::Graphics;
    };

    struct CommandList : public GraphicsDeviceChild {
        CommandList() {
            commandBuffer = makeRef<CommandBuffer>();
        };
        Ref<CommandBuffer> commandBuffer = {};
    };
}