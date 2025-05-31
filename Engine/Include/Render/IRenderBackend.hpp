#pragma once
#include "Render/RenderStructs.hpp"
#include "Render/RenderConstants.hpp"

namespace FS
{
    class IRenderBackend 
    {
    public:
        virtual ~IRenderBackend() = default;
        
        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        
        virtual void Present() = 0;
        virtual void WaitForGPU() = 0;
        virtual void Resize() = 0;

        virtual void OneTimeSubmit(const Span<const CommandHandle>& command_handles, QueueType queue_type) = 0;
        virtual void Submit(const Span<const CommandHandle>& command_handles, QueueType queue_type) = 0;
        virtual void BeginCommand(CommandHandle command_handle) = 0;
        virtual void EndCommand(CommandHandle command_handle) = 0;

        virtual void ClearRenderTarget(CommandHandle command_handle, RenderTargetHandle render_target_handle, glm::vec4 clear_color) = 0;

        [[nodiscard]] virtual RenderTargetHandle CreateRenderTarget(RenderTargetCreateInfo create_info,
                                                            std::string_view debug_name) = 0;
        [[nodiscard]] virtual DepthStencilHandle CreateDepthStencil(DepthStencilCreateInfo create_info,
                                                                    std::string_view debug_name) = 0;
        [[nodiscard]] virtual CommandHandle CreateCommand(QueueType queue_type, std::string_view debug_name) = 0;
        [[nodiscard]] virtual BufferHandle CreateBuffer(const BufferCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const GraphicsShaderCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const ComputeShaderCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;

        virtual void DestroyRenderTarget(RenderTargetHandle render_target_handle) = 0;
        virtual void DestroyDepthStencil(DepthStencilHandle depth_stencil_handle) = 0;
        virtual void DestroyBuffer(BufferHandle buffer_handle) = 0;

        FrameData& GetFrameData() { return m_frame_datas.at(m_frame_index); }

    protected:
        std::array<FrameData, kFrameCount> m_frame_datas = {};
        u32 m_frame_index = 0;
    };
}