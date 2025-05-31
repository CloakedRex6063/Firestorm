#pragma once
#include "Render/RenderEnums.hpp"

namespace FS
{
    struct RenderContextCreateInfo
    {
        DevicePreference GpuPreference;
    };
    enum class CommandHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class RenderTargetHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class DepthStencilHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class BufferHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class TextureHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class ResourceHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };
    enum class ShaderHandle : u32
    {
        eNull = std::numeric_limits<u32>::max(),
    };


    struct RenderPassInfo
    {
        std::vector<RenderTargetHandle> RenderTargets{};
        DepthStencilHandle DepthStencil = DepthStencilHandle::eNull;

        enum class LoadOp
        {
            eClear,
            eLoad,
        };

        enum class StoreOp
        {
            eDiscard,
            eStore
        };

        LoadOp RenderTargetLoadOp = LoadOp::eClear;
        StoreOp RenderTargetStoreOp = StoreOp::eStore;

        LoadOp DepthStencilLoadOp = LoadOp::eClear;
        StoreOp DepthStencilStoreOp = StoreOp::eStore;

        glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float ClearDepth = 1.0f;
    };

    struct FrameData
    {
        CommandHandle CommandHandle = CommandHandle::eNull;
        RenderTargetHandle RenderTargetHandle = RenderTargetHandle::eNull;
        u64 FenceValue = 0;
    };

    struct RenderTargetCreateInfo
    {
        glm::uvec2 Size;
        Format Format;
        ViewType ViewType;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
    };

    struct DepthStencilCreateInfo
    {
        glm::uvec2 Size;
        Format Format;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
    };

    struct BufferCreateInfo
    {
        u64 FirstElement = 0;
        u32 NumElements = 0;
        u32 Stride = 0;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
        BufferType Type = BufferType::eStorage;
    };

    struct TextureCreateInfo
    {
        u64 Width = 0;
        u32 Height = 0;
        u16 Depth = 1;
        u16 MipLevels = 1;
        Format Format{};
        ViewType ViewType{};
        bool RenderTarget = false;
        bool DepthStencil = false;
    };
    
    struct GraphicsShaderCreateInfo
    {
        std::vector<char> VertexCode = {};
        std::vector<char> FragmentCode = {};
        PrimitiveTopology PrimitiveTopology = PrimitiveTopology::eTriangle;
        std::vector<Format> RenderTargetFormats{};
        u32 NumRenderTargets = 0;
        Format DepthStencilFormat = Format::eUnknown;
        FillMode FillMode = FillMode::eSolid;
        CullMode CullMode = CullMode::eBack;
        FrontFace FrontFace = FrontFace::eCounterClockwise;
    };

    struct ComputeShaderCreateInfo
    {
        std::vector<char> ComputeCode = {};
    };
    
    struct Viewport
    {
        glm::vec2 Dimensions;
        glm::vec2 Offset = glm::vec2(0.0f);
        glm::vec2 DepthRange = glm::vec2(0.f, 1.f);
    };

    struct Scissor
    {
        glm::u16vec2 Min = glm::u16vec2(0);
        glm::u16vec2 Max;
    };
} // namespace FS