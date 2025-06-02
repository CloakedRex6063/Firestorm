#pragma once
#include "Render/RenderStructs.hpp"
#include "Tools/Log.hpp"

namespace FS::DX12
{
    template <typename... Args>
    void ThrowIfFailed(const HRESULT hResult, const std::format_string<Args...>& errorMessage, Args&&... args)
    {
        if (FAILED(hResult))
        {
            const _com_error err(hResult);
            FormatString(errorMessage, std::forward<Args>(args)...);
            Log::Error("{}", err.ErrorMessage());
            Log::Critical(errorMessage);
        }
    }

    inline D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE GetLoadOp(const RenderPassInfo::LoadOp loadOp)
    {
        switch (loadOp)
        {
        case RenderPassInfo::LoadOp::eClear:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        case RenderPassInfo::LoadOp::eLoad:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        }
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
    }

    inline D3D12_RENDER_PASS_ENDING_ACCESS_TYPE GetStoreOp(const RenderPassInfo::StoreOp storeOp)
    {
        switch (storeOp)
        {
        case RenderPassInfo::StoreOp::eDiscard:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        case RenderPassInfo::StoreOp::eStore:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        }
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    }

    inline D3D12_RENDER_PASS_RENDER_TARGET_DESC GetRenderTargetDesc(const Texture& renderTarget,
                                                                    const RenderPassInfo::LoadOp loadOp,
                                                                    const RenderPassInfo::StoreOp storeOp,
                                                                    const glm::vec4& clearColor)
    {
        const CD3DX12_CLEAR_VALUE clearValue{DXGI_FORMAT_R32G32B32_FLOAT, glm::value_ptr(clearColor)};
        return {
            .cpuDescriptor = renderTarget.RtvDescriptor.Cpu,
            .BeginningAccess = {.Type = GetLoadOp(loadOp), .Clear = clearValue},
            .EndingAccess = GetStoreOp(storeOp),
        };
    }

    inline D3D12_RENDER_PASS_DEPTH_STENCIL_DESC GetDepthStencilDesc(const Texture& depthTarget,
                                                                    const RenderPassInfo::LoadOp loadOp,
                                                                    const RenderPassInfo::StoreOp storeOp,
                                                                    const float clearColor)
    {
        const CD3DX12_CLEAR_VALUE clearValue{DXGI_FORMAT_R32_FLOAT, &clearColor};
        return {
            .cpuDescriptor = depthTarget.DsvDescriptor.Cpu,
            .DepthBeginningAccess = {.Type = GetLoadOp(loadOp), .Clear = clearValue},
            .StencilBeginningAccess = {.Type = GetLoadOp(loadOp), .Clear = clearValue},
            .DepthEndingAccess = GetStoreOp(storeOp),
            .StencilEndingAccess = GetStoreOp(storeOp),
        };
    }

    constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(const PrimitiveTopology& topology)
    {
        return static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(topology);
    }

    constexpr D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(const PrimitiveTopology& topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::eUndefined:
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            break;
        case PrimitiveTopology::ePoint:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case PrimitiveTopology::eLine:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case PrimitiveTopology::eTriangle:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        }
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }

    constexpr DXGI_FORMAT GetFormat(const Format format) { return static_cast<DXGI_FORMAT>(format); }

    constexpr D3D12_FILL_MODE GetFillMode(const FillMode fill_mode) { return static_cast<D3D12_FILL_MODE>(fill_mode); }

    constexpr D3D12_CULL_MODE GetCullMode(const CullMode cull_mode) { return static_cast<D3D12_CULL_MODE>(cull_mode); }

    constexpr bool GetFrontFace(const FrontFace front_face) { return static_cast<bool>(front_face); }

    constexpr D3D12_RTV_DIMENSION GetRTVDimension(const ViewType view_type)
    {
        return static_cast<D3D12_RTV_DIMENSION>(view_type);
    }

    constexpr D3D12_DSV_DIMENSION GetDSVDimension(const ViewType view_type)
    {
        switch (view_type)
        {
        case ViewType::eUnknown:
        case ViewType::eBuffer:
            return D3D12_DSV_DIMENSION_UNKNOWN;
            break;
        case ViewType::eTexture1D:
            return D3D12_DSV_DIMENSION_TEXTURE1D;
            break;
        case ViewType::eTexture1DArray:
            return D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            break;
        case ViewType::eTexture2D:
            return D3D12_DSV_DIMENSION_TEXTURE2D;
            break;
        case ViewType::eTexture2DArray:
            return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            break;
        case ViewType::eTexture3D:
            return D3D12_DSV_DIMENSION_UNKNOWN;
            break;
        }
        return D3D12_DSV_DIMENSION_UNKNOWN;
    }

    constexpr D3D12_RESOURCE_DIMENSION GetResourceDimension(const ViewType viewType)
    {
        switch (viewType)
        {
        case ViewType::eUnknown:
            return D3D12_RESOURCE_DIMENSION_UNKNOWN;
            break;
        case ViewType::eBuffer:
            return D3D12_RESOURCE_DIMENSION_BUFFER;
            break;
        case ViewType::eTexture1D:
        case ViewType::eTexture1DArray:
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case ViewType::eTexture2D:
        case ViewType::eTexture2DArray:
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case ViewType::eTexture3D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        }
        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }

    constexpr D3D12_SRV_DIMENSION GetSRVDimension(const ViewType viewType)
    {
        switch (viewType)
        {
        case ViewType::eUnknown:
            return D3D12_SRV_DIMENSION_UNKNOWN;
        case ViewType::eBuffer:
            return D3D12_SRV_DIMENSION_BUFFER;
        case ViewType::eTexture1D:
        case ViewType::eTexture1DArray:
            return D3D12_SRV_DIMENSION_TEXTURE1D;
        case ViewType::eTexture2D:
        case ViewType::eTexture2DArray:
            return D3D12_SRV_DIMENSION_TEXTURE2D;
        case ViewType::eTexture3D:
            return D3D12_SRV_DIMENSION_TEXTURE3D;
        }
        return D3D12_SRV_DIMENSION_UNKNOWN;
    }

    inline ID3D12Resource2* GetSwapchainBuffer(IDXGISwapChain4* const swapchain, const u32 index)
    {
        ID3D12Resource2* resource;
        const auto resourceResult = swapchain->GetBuffer(index, IID_PPV_ARGS(&resource));
        DX12::ThrowIfFailed(resourceResult, "Failed to get swap chain buffer");
        return resource;
    }

    inline void Name(ID3D12Object* const object, const std::string_view name)
    {
        const auto wDebugName = std::wstring(name.begin(), name.end());
        const auto nameResult = object->SetName(wDebugName.c_str());
        ThrowIfFailed(nameResult, "Failed to name swap chain buffer resource");
    }

    // This is generic enough that it can be used for other backends, so maybe will move it later
    inline u64 Align(const u64 value, const u64 alignment) { return (value + alignment - 1) & ~(alignment - 1); }
} // namespace FS::DX12