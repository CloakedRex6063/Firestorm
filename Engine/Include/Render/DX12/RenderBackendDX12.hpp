#pragma once
#include "Render/IRenderBackend.hpp"
#include "Render/DX12/RenderStructsDX12.hpp"

namespace FS
{
    class RenderBackendDX12 final : public IRenderBackend
    {
    public:
        void Init() override;
        void Shutdown() override;

        void Present() override;
        void WaitForGPU() override;
        void Resize() override;

        void OneTimeSubmit(const Span<const CommandHandle>& command_handle, QueueType queue_type) override;
        void Submit(const Span<const CommandHandle>& command_handle, QueueType queue_type) override;
        void BeginCommand(CommandHandle commandHandle) override;
        void EndCommand(CommandHandle commandHandle) override;
        void ClearRenderTarget(CommandHandle command_handle, RenderTargetHandle render_target_handle,
                               glm::vec4 clear_color) override;

        [[nodiscard]] RenderTargetHandle CreateRenderTarget(RenderTargetCreateInfo create_info,
                                                            std::string_view debug_name) override;
        [[nodiscard]] DepthStencilHandle CreateDepthStencil(DepthStencilCreateInfo create_info,
                                                            std::string_view debug_name) override;
        [[nodiscard]] CommandHandle CreateCommand(QueueType queue_type, std::string_view debug_name) override;
        [[nodiscard]] BufferHandle
        CreateBuffer(const BufferCreateInfo& create_info, std::string_view debug_name) override;
        [[nodiscard]] ShaderHandle CreateShader(const GraphicsShaderCreateInfo& create_info,
                                                std::string_view debug_name) override;
        [[nodiscard]] ShaderHandle
        CreateShader(const ComputeShaderCreateInfo& create_info, std::string_view debug_name) override;

        void DestroyRenderTarget(RenderTargetHandle render_target_handle) override;
        void DestroyDepthStencil(DepthStencilHandle depth_stencil_handle) override;
        void DestroyBuffer(BufferHandle buffer_handle) override;

    private:
        void ChooseGPU();
        void CreateDevice();
        void CreateQueues();
        void CreateFences();
        void CreateSwapchain();
        void CreateFrameData();
        void CreateDescriptorHeaps();
        void CreateHeaps();
        void CreateRootSignature();

        [[nodiscard]] ResourceHandle
        CreateResource(const DX12::Heap& heap, const TextureCreateInfo& create_info, std::string_view debug_name);
        [[nodiscard]] ResourceHandle
        CreateResource(DX12::Heap& heap, const BufferCreateInfo& create_info, std::string_view debug_name);
        [[nodiscard]] DX12::Heap CreateHeap(D3D12_HEAP_TYPE type, u32 size, std::string_view debug_name) const;
        [[nodiscard]] DX12::DescriptorAllocator CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type,
                                                                     std::string_view debug_name) const;
        [[nodiscard]] DX12::Descriptor CreateShaderResourceView(ResourceHandle resource_handle,
                                                                const BufferCreateInfo& create_info);
        [[nodiscard]] DX12::Descriptor CreateShaderResourceView(ResourceHandle resource_handle,
                                                                const TextureCreateInfo& create_info);
        [[nodiscard]] DX12::Descriptor CreateRenderTargetView(ResourceHandle resource_handle,
                                                              const RenderTargetCreateInfo& create_info);
        [[nodiscard]] DX12::Descriptor CreateDepthStencilView(ResourceHandle resource_handle,
                                                              const DepthStencilCreateInfo& create_info);
        void TransitionResource(CommandHandle command_handle,
                                ResourceHandle resource_handle,
                                D3D12_RESOURCE_STATES new_state);

        RenderContextCreateInfo m_args{};
        IDXGIFactory7* m_factory = nullptr;
        IDXGIAdapter4* m_adapter = nullptr;
        IDXGISwapChain4* m_swap_chain = nullptr;
        ID3D12Device14* m_device = nullptr;
        ID3D12CommandQueue* m_graphics_queue = nullptr;
        ID3D12Fence1* m_graphics_fence = nullptr;
        ID3D12CommandQueue* m_transfer_queue = nullptr;
        ID3D12Fence1* m_transfer_fence = nullptr;
        u64 m_transfer_fence_value = 0;
        ID3D12RootSignature* m_root_signature = nullptr;

        DX12::DescriptorAllocator m_cbv_uav_srv_allocator = {};
        DX12::DescriptorAllocator m_rtv_allocator = {};
        DX12::DescriptorAllocator m_dsv_allocator = {};

        DX12::Heap m_buffer_heap;
        DX12::Heap m_texture_heap;
        DX12::Heap m_upload_heap;
        DX12::Heap m_readback_heap;

        std::vector<DX12::Command> m_commands;
        std::vector<DX12::RenderTarget> m_render_targets;
        std::vector<DX12::DepthStencil> m_depth_stencils;
        std::vector<DX12::Texture> m_textures;
        std::vector<DX12::Buffer> m_buffers;
        std::vector<ID3D12PipelineState*> m_shaders;
        std::vector<DX12::Resource> m_resources;
    };
}
