#include "Render/DX12/RenderBackendDX12.hpp"
#include "Core/Window.hpp"
#include "Render/DX12/HelpersDX12.hpp"
#include "Tools/Log.hpp"

void FS::RenderBackendDX12::Init()
{
    ChooseGPU();
    CreateDevice();
    CreateQueues();
    CreateSwapchain();
    CreateDescriptorHeaps();
    CreateHeaps();
    CreateFrameData();
    CreateRootSignature();
    CreateFences();
    Log::Info("Render Backend Initialized");
}

void FS::RenderBackendDX12::Shutdown()
{
    WaitForGPU();
}

void FS::RenderBackendDX12::Present()
{
    const auto present_result = m_swap_chain->Present(0, 0);
    DX12::ThrowIfFailed(present_result, "RenderContextDX12::Present Failed to present");

    const auto current_fence_value = GetFrameData().FenceValue;
    auto result = m_graphics_queue->Signal(m_graphics_fence, current_fence_value);
    DX12::ThrowIfFailed(result, "Failed to signal Graphics Queue");

    if (m_graphics_fence->GetCompletedValue() < current_fence_value)
    {
        result = m_graphics_fence->SetEventOnCompletion(current_fence_value, nullptr);
        DX12::ThrowIfFailed(result, "Failed to wait on Graphics Fence");
    }
    m_frame_index = m_swap_chain->GetCurrentBackBufferIndex();
    GetFrameData().FenceValue++;
}

void FS::RenderBackendDX12::WaitForGPU()
{
    auto& fence_value = GetFrameData().FenceValue;
    auto result = m_graphics_queue->Signal(m_graphics_fence, ++fence_value);
    DX12::ThrowIfFailed(result, "Failed to signal Graphics Queue");
    result = m_graphics_fence->SetEventOnCompletion(fence_value, nullptr);
    DX12::ThrowIfFailed(result, "Failed to wait on Graphics Fence");

    result = m_transfer_queue->Signal(m_transfer_fence, ++m_transfer_fence_value);
    DX12::ThrowIfFailed(result, "Failed to signal Transfer Queue");
    result = m_transfer_fence->SetEventOnCompletion(m_transfer_fence_value, nullptr);
    DX12::ThrowIfFailed(result, "Failed to wait on Transfer Fence");
}

void FS::RenderBackendDX12::Resize()
{
    WaitForGPU();
    for (const auto& frame_data : m_frame_datas)
    {
        DestroyRenderTarget(frame_data.RenderTargetHandle);
    }
    const auto size = Window::GetWindowSize();

    const auto result = m_swap_chain->ResizeBuffers(
        kFrameCount, size.x, size.y, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
    DX12::ThrowIfFailed(result, "RenderContextDX12::Resize Failed to resize Swapchain");

    for (const auto& [index, frameData] : std::views::enumerate(m_frame_datas))
    {
        const auto resource = DX12::GetSwapchainBuffer(m_swap_chain, index);
        const auto debug_name = std::string("Swapchain Buffer") + std::to_string(index);
        DX12::Name(resource, debug_name);
        const auto resource_handle = static_cast<ResourceHandle>(m_resources.size());
        m_resources.emplace_back(resource);

        const RenderTargetCreateInfo create_info{
            .Size = Window::GetWindowSize(),
            .Format = Format::eB8G8R8A8_UNORM,
            .ViewType = ViewType::eTexture2D,
            .ResourceHandle = resource_handle,
        };
        frameData.RenderTargetHandle = CreateRenderTarget(create_info, debug_name);
    }
    m_frame_index = 0;
}

void FS::RenderBackendDX12::OneTimeSubmit(const Span<const CommandHandle>& command_handle, const QueueType queue_type)
{
    Submit(command_handle, queue_type);
    switch (queue_type)
    {
    case QueueType::eGraphics:
        {
            const auto signal_result = m_graphics_queue->Signal(m_graphics_fence, GetFrameData().FenceValue);
            DX12::ThrowIfFailed(signal_result, "RenderContextDX12::OneTimeSubmit Failed to signal graphics queue");
            const auto wait_result = m_graphics_fence->SetEventOnCompletion(GetFrameData().FenceValue, nullptr);
            DX12::ThrowIfFailed(wait_result, "RenderContextDX12::WaitForGPU Failed to wait on this frame's work");
            GetFrameData().FenceValue++;
            break;
        }

    case QueueType::eTransfer:
        {
            const auto signal_result = m_transfer_queue->Signal(m_transfer_fence, m_transfer_fence_value);
            DX12::ThrowIfFailed(signal_result, "RenderContextDX12::OneTimeSubmit Failed to signal transfer queue");
            const auto wait_result = m_transfer_fence->SetEventOnCompletion(m_transfer_fence_value, nullptr);
            DX12::ThrowIfFailed(wait_result, "RenderContextDX12::WaitForGPU Failed to wait on transfer queue");
            m_transfer_fence_value++;
            break;
        }
    }
}

void FS::RenderBackendDX12::Submit(const Span<const CommandHandle>& command_handle, const QueueType queue_type)
{
    std::array<ID3D12CommandList*, 4> commands{};
    for (const auto& [index, commandHandle] : std::views::enumerate(command_handle))
    {
        const auto& [CommandAllocator, CommandList] = m_commands.at(static_cast<u32>(commandHandle));
        commands[index] = CommandList;
    }

    switch (queue_type)
    {
    case QueueType::eGraphics:
        m_graphics_queue->ExecuteCommandLists(command_handle.size(), commands.data());
        break;
    case QueueType::eTransfer:
        m_transfer_queue->ExecuteCommandLists(command_handle.size(), commands.data());
        break;
    }
}

void FS::RenderBackendDX12::BeginCommand(CommandHandle commandHandle)
{
    const auto& [CommandAllocator, CommandList] = m_commands.at(static_cast<u32>(commandHandle));
    const auto allocResult = CommandAllocator->Reset();
    DX12::ThrowIfFailed(allocResult, "RenderContextDX12::BeginCommand Failed to reset command allocator");
    const auto listResult = CommandList->Reset(CommandAllocator, nullptr);
    DX12::ThrowIfFailed(listResult, "RenderContextDX12::BeginCommand Failed to reset command");
}

void FS::RenderBackendDX12::EndCommand(CommandHandle commandHandle)
{
    const auto& renderTarget = m_render_targets.at(static_cast<u32>(GetFrameData().RenderTargetHandle));
    TransitionResource(commandHandle, renderTarget.ResourceHandle, D3D12_RESOURCE_STATE_PRESENT);
    const auto& [CommandAllocator, CommandList] = m_commands.at(static_cast<u32>(commandHandle));
    const auto listResult = CommandList->Close();
    DX12::ThrowIfFailed(listResult, "RenderContextDX12::EndCommand Failed to close command list");
}

void FS::RenderBackendDX12::ClearRenderTarget(CommandHandle command_handle, RenderTargetHandle render_target_handle,
                                              glm::vec4 clear_color)
{
    const auto& command = m_commands[static_cast<u32>(command_handle)].CommandList;
    const auto& render_target = m_render_targets[static_cast<u32>(command_handle)];
    TransitionResource(command_handle, render_target.ResourceHandle, D3D12_RESOURCE_STATE_RENDER_TARGET);
    command->ClearRenderTargetView(render_target.RenderDescriptor.Cpu, glm::value_ptr(clear_color), 0, nullptr);
}

FS::RenderTargetHandle FS::RenderBackendDX12::CreateRenderTarget(const RenderTargetCreateInfo create_info,
                                                                 const std::string_view debug_name)
{
    DX12::RenderTarget rt;
    rt.ResourceHandle = create_info.ResourceHandle;
    const TextureCreateInfo texture_create_info{
        .Width = create_info.Size.x,
        .Height = create_info.Size.y,
        .Depth = 1,
        .MipLevels = 1,
        .Format = create_info.Format,
        .ViewType = create_info.ViewType,
        .RenderTarget = true,
    };
    if (rt.ResourceHandle == ResourceHandle::eNull)
    {
        rt.ResourceHandle = CreateResource(m_texture_heap, texture_create_info, debug_name);
    }
    rt.RenderDescriptor = CreateRenderTargetView(rt.ResourceHandle, create_info);
    rt.TextureDescriptor = CreateShaderResourceView(rt.ResourceHandle, texture_create_info);
    const auto handle = static_cast<RenderTargetHandle>(m_render_targets.size());
    m_render_targets.emplace_back(rt);
    return handle;
}

FS::DepthStencilHandle FS::RenderBackendDX12::CreateDepthStencil(const DepthStencilCreateInfo create_info,
                                                                 const std::string_view debug_name)
{
    DX12::DepthStencil ds;
    ds.ResourceHandle = create_info.ResourceHandle;
    const TextureCreateInfo texture_create_info{
        .Width = create_info.Size.x,
        .Height = create_info.Size.y,
        .Depth = 1,
        .MipLevels = 1,
        .Format = create_info.Format,
        .ViewType = ViewType::eTexture2D,
        .DepthStencil = true,
    };
    if (ds.ResourceHandle == ResourceHandle::eNull)
    {
        ds.ResourceHandle = CreateResource(m_texture_heap, texture_create_info, debug_name);
    }
    ds.DepthDescriptor = CreateDepthStencilView(ds.ResourceHandle, create_info);
    ds.TextureDescriptor = CreateShaderResourceView(ds.ResourceHandle, texture_create_info);
    const auto handle = static_cast<DepthStencilHandle>(m_render_targets.size());
    m_depth_stencils.emplace_back(ds);
    return handle;
}

FS::CommandHandle FS::RenderBackendDX12::CreateCommand(const QueueType queue_type, std::string_view debug_name)
{
    D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
    switch (queue_type)
    {
    case QueueType::eGraphics:
        commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case QueueType::eTransfer:
        commandListType = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    }
    DX12::Command command;
    const auto allocResult =
        m_device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&command.CommandAllocator));
    DX12::ThrowIfFailed(allocResult, "RenderContextDX12::CreateCommand Failed to create command allocator");
    const auto listResult = m_device->CreateCommandList(
        0, commandListType, command.CommandAllocator, nullptr, IID_PPV_ARGS(&command.CommandList));
    DX12::ThrowIfFailed(listResult, "RenderContextDX12::CreateCommand Failed to create command list");
    m_commands.emplace_back(command);
    const auto closeResult = m_commands.back().CommandList->Close();
    DX12::ThrowIfFailed(closeResult, "RenderContextDX12::CreateCommand Failed to close command list");

    const auto w_debug_name = std::wstring(debug_name.begin(), debug_name.end());
    const auto nameAllocResult = command.CommandAllocator->SetName(w_debug_name.c_str());
    DX12::ThrowIfFailed(nameAllocResult, "RenderContextDX12::CreateCommand Failed to name command allocator");

    const auto nameListResult = command.CommandList->SetName(w_debug_name.c_str());
    DX12::ThrowIfFailed(nameListResult, "RenderContextDX12::CreateCommand Failed to name command list");

    return static_cast<CommandHandle>(m_commands.size() - 1);
}

FS::BufferHandle FS::RenderBackendDX12::CreateBuffer(const BufferCreateInfo& create_info, std::string_view debug_name)
{
    DX12::Buffer buffer{};

    buffer.ResourceHandle = create_info.ResourceHandle;
    if (create_info.ResourceHandle == ResourceHandle::eNull)
    {
        DX12::Heap heap;
        switch (create_info.Type)
        {
        case BufferType::eStorage:
            heap = m_buffer_heap;
        case BufferType::eUniform:
            break;
        case BufferType::eStaging:
            heap = m_upload_heap;
            break;
        case BufferType::eReadback:
            heap = m_readback_heap;
            break;
        }
        buffer.ResourceHandle = CreateResource(heap, create_info, debug_name);
    }
    buffer.Descriptor = CreateShaderResourceView(buffer.ResourceHandle, create_info);

    const auto handle = static_cast<BufferHandle>(m_buffers.size());
    m_buffers.emplace_back(buffer);
    return handle;
}

FS::ShaderHandle FS::RenderBackendDX12::CreateShader(const GraphicsShaderCreateInfo& create_info,
                                                     std::string_view debug_name)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{
        .pRootSignature = m_root_signature,
        .VS =
        {
            .pShaderBytecode = create_info.VertexCode.data(),
            .BytecodeLength = create_info.VertexCode.size(),
        },
        .PS =
        {
            .pShaderBytecode = create_info.FragmentCode.data(),
            .BytecodeLength = create_info.FragmentCode.size(),
        },
        .BlendState = CD3DX12_BLEND_DESC{D3D12_DEFAULT},
        .SampleMask = D3D12_DEFAULT_SAMPLE_MASK,
        .RasterizerState =
        {
            .FillMode = DX12::GetFillMode(create_info.FillMode),
            .CullMode = DX12::GetCullMode(create_info.CullMode),
            .FrontCounterClockwise = DX12::GetFrontFace(create_info.FrontFace),
        },
        .DepthStencilState =
        {
            .DepthEnable = create_info.DepthStencilFormat != Format::eUnknown,
            .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
        },
        .PrimitiveTopologyType = DX12::GetPrimitiveTopologyType(create_info.PrimitiveTopology),
        .NumRenderTargets = create_info.NumRenderTargets,
        .DSVFormat = DX12::GetFormat(create_info.DepthStencilFormat),
        .SampleDesc = {.Count = 1, .Quality = 0},
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE,
    };

    for (int i = 0; i < create_info.NumRenderTargets; i++)
    {
        desc.RTVFormats[i] = DX12::GetFormat(create_info.RenderTargetFormats[i]);
    }
    ID3D12PipelineState* pipelineState;
    const auto result = m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
    DX12::ThrowIfFailed(result, "RenderContextDX12::CreateGraphicsShader Failed to create graphics pipeline");
    const auto w_debug_name = std::wstring(debug_name.begin(), debug_name.end());
    const auto nameResult = pipelineState->SetName(w_debug_name.c_str());
    DX12::ThrowIfFailed(nameResult, "RenderContextDX12::CreateGraphicsShader Failed to name compute pipeline");
    const auto shaderHandle = static_cast<ShaderHandle>(m_shaders.size());
    m_shaders.emplace_back(pipelineState);
    return shaderHandle;
}

FS::ShaderHandle FS::RenderBackendDX12::CreateShader(const ComputeShaderCreateInfo& create_info,
                                                     std::string_view debug_name)
{
    const D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {
        .pRootSignature = m_root_signature,
        .CS =
        {
            create_info.ComputeCode.data(),
            create_info.ComputeCode.size(),
        },
    };
    ID3D12PipelineState* pipelineState;
    const auto result = m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineState));
    DX12::ThrowIfFailed(result, "RenderContextDX12::CreateComputeShader Failed to create compute pipeline");
    const auto wDebugName = std::wstring(debug_name.begin(), debug_name.end());
    const auto nameResult = pipelineState->SetName(wDebugName.c_str());
    DX12::ThrowIfFailed(nameResult, "RenderContextDX12::CreateComputeShader Failed to name compute pipeline");
    const auto shaderHandle = static_cast<ShaderHandle>(m_shaders.size());
    m_shaders.emplace_back(pipelineState);
    return shaderHandle;
}

void FS::RenderBackendDX12::DestroyRenderTarget(RenderTargetHandle render_target_handle)
{
    const auto& renderTarget = m_render_targets.at(static_cast<u32>(render_target_handle));
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(renderTarget.ResourceHandle));
    m_rtv_allocator.Free(renderTarget.RenderDescriptor);
    m_cbv_uav_srv_allocator.Free(renderTarget.TextureDescriptor);
    BaseResource->Release();
}

void FS::RenderBackendDX12::DestroyDepthStencil(DepthStencilHandle depth_stencil_handle)
{
    const auto& [DepthDescriptor, TextureDescriptor, ResourceHandle] = m_depth_stencils.at(
        static_cast<u32>(depth_stencil_handle));
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(ResourceHandle));
    m_dsv_allocator.Free(DepthDescriptor);
    m_cbv_uav_srv_allocator.Free(TextureDescriptor);
    BaseResource->Release();
}

void FS::RenderBackendDX12::DestroyBuffer(BufferHandle buffer_handle)
{
    const auto& buffer = m_buffers.at(static_cast<u32>(buffer_handle));
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(buffer.ResourceHandle));
    m_cbv_uav_srv_allocator.Free(buffer.Descriptor);
    BaseResource->Release();
}

void FS::RenderBackendDX12::ChooseGPU()
{
    u32 flags = 0;
#ifdef NEO_DEBUG
        flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    const auto result = CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory));
    DX12::ThrowIfFailed(result, "Failed to create DXGIFactory2");
    Log::Info("Created DXGIFactory2");
    DXGI_GPU_PREFERENCE preference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    switch (m_args.GpuPreference)
    {
    case DevicePreference::eHighPerformance:
        preference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
        break;
    case DevicePreference::ePowerSaving:
        preference = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
        break;
    }

#ifdef FS_DEBUG
    ID3D12Debug6* debug_controller;
    const auto debug_result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
    DX12::ThrowIfFailed(debug_result, "RenderContextDX12::ChooseGPU Failed to get debug interface");
    debug_controller->EnableDebugLayer();
#endif
    IDXGIAdapter1* adapter;
    for (UINT i = 0; m_factory->EnumAdapterByGpuPreference(i, preference, IID_PPV_ARGS(&adapter)) !=
         DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        [[maybe_unused]] auto desc_result = adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
        {
            const auto query_result = adapter->QueryInterface(IID_PPV_ARGS(&m_adapter));
            DX12::ThrowIfFailed(query_result, "Failed to query adapter");
            adapter->Release();
            break;
        }
    }
    if (!m_adapter)
    {
        ThrowError("No suitable GPU found");
    }

    DXGI_ADAPTER_DESC3 desc{};
    const auto desc_result = m_adapter->GetDesc3(&desc);
    DX12::ThrowIfFailed(desc_result, "Failed to get Adapter description");

    FS_WARN_BEG()
    FS_WARN_WCONV()
    const auto description = std::string(std::begin(desc.Description), std::end(desc.Description));
    FS_WARN_END()
    Log::Info("Adapter description:");
    Log::Info("{}", description);
    Log::Info("Shared Memory: {} GB", static_cast<float>(desc.SharedSystemMemory) / 1024 / 1024 / 1024);
    Log::Info("Dedicated Memory: {} GB", static_cast<float>(desc.DedicatedVideoMemory) / 1024 / 1024 / 1024);
    Log::Info("Total Memory: {} GB",
              static_cast<float>(desc.SharedSystemMemory + desc.DedicatedVideoMemory) / 1024 / 1024 / 1024);
}

void FS::RenderBackendDX12::CreateDevice()
{
    const auto device_create_result = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
    DX12::ThrowIfFailed(device_create_result, "Failed to create device");
    Log::Info("Created device");

#ifdef FS_DEBUG
    ID3D12InfoQueue1* info_queue;
    const auto result = m_device->QueryInterface(IID_PPV_ARGS(&info_queue));
    DX12::ThrowIfFailed(result, "RenderContextDX12::CreateDevice Failed to create info queue");
    DWORD callback_cookie = 0;
    const auto register_result = info_queue->RegisterMessageCallback(
        [](D3D12_MESSAGE_CATEGORY,
           const D3D12_MESSAGE_SEVERITY severity,
           D3D12_MESSAGE_ID,
           const LPCSTR p_description,
           void*)
        {
            const auto description = std::string(p_description);
            switch (severity)
            {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION:
                Log::Critical("[DX12] {}", description);
                break;
            case D3D12_MESSAGE_SEVERITY_ERROR:
                Log::Error("[DX12] {}", description);
                break;
            case D3D12_MESSAGE_SEVERITY_WARNING:
                Log::Warn("[DX12] {}", description);
                break;
            case D3D12_MESSAGE_SEVERITY_INFO:
            case D3D12_MESSAGE_SEVERITY_MESSAGE:
                break;
            }
        },
        D3D12_MESSAGE_CALLBACK_FLAG_NONE,
        nullptr,
        &callback_cookie);
    DX12::ThrowIfFailed(register_result, "RenderContextDX12::CreateDevice Failed to register message callback");
#endif
}

void FS::RenderBackendDX12::CreateQueues()
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
    };
    const auto graphics_result = m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_graphics_queue));
    DX12::ThrowIfFailed(graphics_result, "Failed to create graphics queue");

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
    const auto transfer_result = m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_transfer_queue));
    DX12::ThrowIfFailed(transfer_result, "Failed to create transfer queue");
}

void FS::RenderBackendDX12::CreateFences()
{
    const auto graphics_result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_graphics_fence));
    DX12::ThrowIfFailed(graphics_result, "Failed to create graphics fence");

    const auto transfer_fence = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_transfer_fence));
    DX12::ThrowIfFailed(transfer_fence, "Failed to create transfer fence");
}

void FS::RenderBackendDX12::CreateSwapchain()
{
    const auto hwnd = static_cast<HWND>(Window::GetHandle());
    const glm::uvec2 window_size = Window::GetWindowSize();
    const DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {
        .Width = window_size.x,
        .Height = window_size.y,
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .Stereo = false,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 3,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
    };
    IDXGISwapChain1* swap_chain;
    const auto create_result =
        m_factory->CreateSwapChainForHwnd(m_graphics_queue, hwnd, &swap_chain_desc, nullptr, nullptr, &swap_chain);
    DX12::ThrowIfFailed(create_result, "Failed to create swap chain");
    const auto cast_result = swap_chain->QueryInterface(IID_PPV_ARGS(&m_swap_chain));
    DX12::ThrowIfFailed(cast_result, "Failed to get swap chain");
    Log::Info("Created swap chain");
}

void FS::RenderBackendDX12::CreateFrameData()
{
    for (auto [index, frameData] : std::views::enumerate(m_frame_datas))
    {
        frameData.CommandHandle = CreateCommand(QueueType::eGraphics, "Frame Command" + std::to_string(index));

        const auto resource = DX12::GetSwapchainBuffer(m_swap_chain, index);
        const auto debug_name = std::string("Swapchain Buffer") + std::to_string(index);
        DX12::Name(resource, debug_name);
        const auto resource_handle = static_cast<ResourceHandle>(m_resources.size());
        m_resources.emplace_back(resource);

        const RenderTargetCreateInfo create_info{
            .Size = Window::GetWindowSize(),
            .Format = Format::eB8G8R8A8_UNORM,
            .ViewType = ViewType::eTexture2D,
            .ResourceHandle = resource_handle,
        };
        frameData.RenderTargetHandle = CreateRenderTarget(create_info, debug_name);
    }
    m_frame_index = m_swap_chain->GetCurrentBackBufferIndex();
}

void FS::RenderBackendDX12::CreateDescriptorHeaps()
{
    m_rtv_allocator = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, "RTV Descriptor Heap");
    m_dsv_allocator = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, "DSV Descriptor Heap");
    m_cbv_uav_srv_allocator =
        CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "CBV_SRV_UAV Descriptor Heap");
}

void FS::RenderBackendDX12::CreateHeaps()
{
    m_buffer_heap = CreateHeap(D3D12_HEAP_TYPE_DEFAULT, 1 * 1024 * 1024 * 1024, "Buffer Heap");
    m_texture_heap = CreateHeap(D3D12_HEAP_TYPE_DEFAULT, 1 * 1024 * 1024 * 1024, "Texture Heap");
    m_upload_heap = CreateHeap(D3D12_HEAP_TYPE_UPLOAD, 200 * 1024 * 1024, "Upload Heap");
    m_readback_heap = CreateHeap(D3D12_HEAP_TYPE_READBACK, 10 * 1024 * 1024, "Readback Heap");
}

void FS::RenderBackendDX12::CreateRootSignature()
{
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;

    constexpr std::array parameters = {
        D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            .Constants =
            {
                .ShaderRegister = 0,
                .RegisterSpace = 0,
                .Num32BitValues = 58, // 58 because each root cbv takes 2 uints, and the max is 64
            },
        },
        D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
            .Descriptor =
            {
                .ShaderRegister = 1,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
            },
        },
        D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
            .Descriptor =
            {
                .ShaderRegister = 2,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
            },
        },
        D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
            .Descriptor =
            {
                .ShaderRegister = 3,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
            },
        },
    };

    constexpr D3D12_STATIC_SAMPLER_DESC sampler = {
        // s_nearest
        .Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,
        .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .MipLODBias = 0.0f,
        .MinLOD = 0.0f,
        .MaxLOD = D3D12_FLOAT32_MAX,
        .ShaderRegister = 0,
        .RegisterSpace = 0,
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
    };

    root_signature_desc.Init_1_1(parameters.size(),
                                 parameters.data(),
                                 1,
                                 &sampler,
                                 D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);
    ID3DBlob* signature;
    ID3DBlob* error;
    const auto serialize_result = D3D12SerializeVersionedRootSignature(&root_signature_desc, &signature, &error);
    DX12::ThrowIfFailed(serialize_result, "Failed to serialize root signature");
    const auto signature_result = m_device->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signature));
    DX12::ThrowIfFailed(signature_result, "Failed to create root signature");
}

FS::ResourceHandle FS::RenderBackendDX12::CreateResource(const DX12::Heap& heap, const TextureCreateInfo& create_info,
                                                         const std::string_view debug_name)
{
    D3D12_RESOURCE_FLAGS flags = {};
    D3D12_CLEAR_VALUE clear_value{};
    auto clear_color = glm::vec4(0, 0, 0, 0);
    constexpr float clear_depth = 1.0f;
    if (create_info.RenderTarget)
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        clear_value = CD3DX12_CLEAR_VALUE{DX12::GetFormat(create_info.Format), glm::value_ptr(clear_color)};
    }
    else if (create_info.DepthStencil)
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        clear_value = CD3DX12_CLEAR_VALUE{DXGI_FORMAT_R32_FLOAT, &clear_depth};
    }
    const D3D12_RESOURCE_DESC resource_desc{
        .Dimension = DX12::GetResourceDimension(create_info.ViewType),
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = create_info.Width,
        .Height = create_info.Height,
        .DepthOrArraySize = create_info.Depth,
        .MipLevels = create_info.MipLevels,
        .Format = DX12::GetFormat(create_info.Format),
        .SampleDesc = {.Count = 1, .Quality = 0},
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = flags,
    };
    ID3D12Resource2* resource;
    const auto resource_result = m_device->CreatePlacedResource(heap.BaseHeap,
                                                                heap.Offset,
                                                                &resource_desc,
                                                                D3D12_RESOURCE_STATE_COMMON,
                                                                &clear_value,
                                                                IID_PPV_ARGS(&resource));
    DX12::Name(resource, debug_name);
    DX12::ThrowIfFailed(resource_result, "RenderContextDX12::CreateResource Failed to create texture resource");

    const auto handle = static_cast<ResourceHandle>(m_resources.size());
    m_resources.emplace_back(resource);
    return handle;
}

FS::ResourceHandle FS::RenderBackendDX12::CreateResource(DX12::Heap& heap, const BufferCreateInfo& create_info,
                                                         std::string_view debug_name)
{
    const D3D12_RESOURCE_DESC resource_desc{
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = create_info.NumElements * create_info.Stride,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };
    ID3D12Resource2* resource;
    const auto resource_result = m_device->CreatePlacedResource(
        heap.BaseHeap, heap.Offset, &resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource));
    const auto w_debug_name = std::wstring(debug_name.begin(), debug_name.end());
    DX12::ThrowIfFailed(resource_result, "RenderContextDX12::CreateResource Failed to create buffer resource");

    const auto [SizeInBytes, Alignment] = m_device->GetResourceAllocationInfo(0, 1, &resource_desc);
    const u64 aligned_offset = DX12::Align(heap.Offset, Alignment);
    heap.Offset = aligned_offset + SizeInBytes;

    const auto handle = static_cast<ResourceHandle>(m_resources.size());
    m_resources.emplace_back(resource);
    return handle;
}

FS::DX12::Heap FS::RenderBackendDX12::CreateHeap(const D3D12_HEAP_TYPE type, const u32 size,
                                                 const std::string_view debug_name) const
{
    D3D12_HEAP_PROPERTIES props;
    switch (type)
    {
    case D3D12_HEAP_TYPE_DEFAULT:
    default:
        {
            props = D3D12_HEAP_PROPERTIES{
                .Type = D3D12_HEAP_TYPE_GPU_UPLOAD,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            };
        }
        break;
    case D3D12_HEAP_TYPE_UPLOAD:
        {
            props = D3D12_HEAP_PROPERTIES{
                .Type = D3D12_HEAP_TYPE_UPLOAD,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            };
        }
        break;
    case D3D12_HEAP_TYPE_READBACK:
        {
            props = D3D12_HEAP_PROPERTIES{
                .Type = D3D12_HEAP_TYPE_READBACK,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
            };
        }
    }
    const D3D12_HEAP_DESC desc = {
        .SizeInBytes = size,
        .Properties = props,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Flags = D3D12_HEAP_FLAG_NONE,
    };
    DX12::Heap heap{};
    heap.Size = size;
    const auto result = m_device->CreateHeap(&desc, IID_PPV_ARGS(&heap.BaseHeap));
    DX12::ThrowIfFailed(result, "Failed to create heap");

    DX12::Name(heap.BaseHeap, debug_name);
    return heap;
}

FS::DX12::DescriptorAllocator FS::RenderBackendDX12::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type,
                                                                          std::string_view debug_name) const
{
    bool shader_visible = false;
    u32 num_descriptors = 0;
    switch (heap_type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        shader_visible = true;
        num_descriptors = 4096;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
    default:
        shader_visible = false;
        num_descriptors = 64;
        break;
    }

    DX12::DescriptorAllocator descriptor_allocator;

    const D3D12_DESCRIPTOR_HEAP_DESC desc = {
        .Type = heap_type,
        .NumDescriptors = num_descriptors,
        .Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
    };

    const auto result = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_allocator.Heap));
    DX12::ThrowIfFailed(result, "RenderContextDX12::CreateDescriptorHeap Failed to create descriptor heap");

    const auto w_name = std::wstring(debug_name.begin(), debug_name.end());
    const auto name_result = descriptor_allocator.Heap->SetName(w_name.c_str());
    DX12::ThrowIfFailed(name_result, "RenderContextDX12::CreateDescriptorHeap Failed to name descriptor heap");

    descriptor_allocator.CpuBase = descriptor_allocator.Heap->GetCPUDescriptorHandleForHeapStart();

    if (shader_visible)
    {
        descriptor_allocator.GpuBase = descriptor_allocator.Heap->GetGPUDescriptorHandleForHeapStart();
    }

    descriptor_allocator.Stride = m_device->GetDescriptorHandleIncrementSize(heap_type);

    descriptor_allocator.Type = heap_type;
    descriptor_allocator.At = 0;
    descriptor_allocator.Capacity = num_descriptors;
    return descriptor_allocator;
}

FS::DX12::Descriptor FS::RenderBackendDX12::CreateShaderResourceView(ResourceHandle resource_handle,
                                                                     const BufferCreateInfo& create_info)
{
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(resource_handle));
    const D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {
        .Format = DXGI_FORMAT_UNKNOWN,
        .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
        .Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Buffer = {
            .FirstElement = create_info.FirstElement,
            .NumElements = create_info.NumElements,
            .StructureByteStride = create_info.Stride,
            .Flags = D3D12_BUFFER_SRV_FLAG_NONE,
        }
    };
    const auto srv_descriptor = m_cbv_uav_srv_allocator.Allocate();
    m_device->CreateShaderResourceView(BaseResource, &view_desc, srv_descriptor.Cpu);
    return srv_descriptor;
}

FS::DX12::Descriptor FS::RenderBackendDX12::CreateShaderResourceView(ResourceHandle resource_handle,
                                                                     const TextureCreateInfo& create_info)
{
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(resource_handle));
    const D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {
        .Format = DX12::GetFormat(create_info.Format),
        .ViewDimension = DX12::GetSRVDimension(create_info.ViewType),
        .Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Texture2D = {
            .MostDetailedMip = 0,
            .MipLevels = create_info.MipLevels,
            .PlaneSlice = 0,
            .ResourceMinLODClamp = 0,
        }
    };
    const auto srv_descriptor = m_cbv_uav_srv_allocator.Allocate();
    m_device->CreateShaderResourceView(BaseResource, &view_desc, srv_descriptor.Cpu);
    return srv_descriptor;
}

FS::DX12::Descriptor FS::RenderBackendDX12::CreateRenderTargetView(ResourceHandle resource_handle,
                                                                   const RenderTargetCreateInfo& create_info)
{
    const D3D12_RENDER_TARGET_VIEW_DESC render_target_view_desc = {
        .Format = DX12::GetFormat(create_info.Format),
        .ViewDimension = DX12::GetRTVDimension(create_info.ViewType),
    };
    const auto rtv_descriptor = m_rtv_allocator.Allocate();
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(resource_handle));
    m_device->CreateRenderTargetView(BaseResource, &render_target_view_desc, rtv_descriptor.Cpu);
    return rtv_descriptor;
}

FS::DX12::Descriptor FS::RenderBackendDX12::CreateDepthStencilView(ResourceHandle resource_handle,
                                                                   const DepthStencilCreateInfo& create_info)
{
    const D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {
        .Format = DX12::GetFormat(create_info.Format),
        .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, // Hardcoded for now, since I don't expect to use anything else
    };
    const auto dsv_descriptor = m_dsv_allocator.Allocate();
    const auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(resource_handle));
    m_device->CreateDepthStencilView(BaseResource, &depth_stencil_view_desc, dsv_descriptor.Cpu);
    return dsv_descriptor;
}

void FS::RenderBackendDX12::TransitionResource(CommandHandle command_handle, ResourceHandle resource_handle,
                                               const D3D12_RESOURCE_STATES new_state)
{
    auto& [BaseResource, ResourceState] = m_resources.at(static_cast<u32>(resource_handle));
    if (ResourceState == new_state)
        return;
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(BaseResource, ResourceState, new_state);
    ResourceState = new_state;
    m_commands.at(static_cast<u32>(command_handle)).CommandList->ResourceBarrier(1, &barrier);
}
