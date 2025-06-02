#pragma once
#include "Render/RenderEnums.hpp"
#include "Tools/EnumFlags.hpp"

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
        std::vector<TextureHandle> RenderTargets{};
        TextureHandle DepthStencil = TextureHandle::eNull;

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
        TextureHandle RenderTargetHandle = TextureHandle::eNull;
        u64 FenceValue = 0;
    };

    struct BufferUploadInfo
    {
        const void* Data = nullptr;
        u32 Size = 0;
        u32 Offset = 0;
    };

    struct BufferCreateInfo
    {
        u64 FirstElement = 0;
        u32 NumElements = 0;
        u32 Stride = 0;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
        BufferType Type = BufferType::eStorage;
        BufferUploadInfo UploadInfo;
    };

    enum class TextureFlags : u8
    {
        eNone = 0,
        eRenderTexture = 1 << 0,
        eDepthTexture = 1 << 1,
        eShaderResource = 1 << 2
    };
    
    struct TextureCreateInfo
    {
        glm::uvec2 Dimensions;
        u16 Depth = 1;
        u16 MipLevels = 1;
        Format Format{};
        ViewType ViewType{};
        EnumFlags<TextureFlags> TextureFlags;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
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

    struct Vertex
    {
        glm::vec3 Position;
        float UVx;
        glm::vec3 Normal;
        float UVy;
        glm::vec4 Tangent;
    };
} // namespace FS
