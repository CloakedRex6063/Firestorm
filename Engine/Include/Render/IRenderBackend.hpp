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

        virtual void PushConstant(CommandHandle commandHandle, u32 count, const void* data) = 0;
        virtual void SetViewport(CommandHandle commandHandle, const Viewport& viewport) = 0;
        virtual void SetScissor(CommandHandle commandHandle, const Scissor& scissor) = 0;
        virtual void SetPrimitiveTopology(CommandHandle commandHandle, PrimitiveTopology topology) = 0;
        virtual void BeginRenderPass(CommandHandle commandHandle, const RenderPassInfo& renderPassInfo) = 0;
        virtual void EndRenderPass(CommandHandle commandHandle) = 0;
        virtual void BindShader(CommandHandle commandHandle, ShaderHandle shaderHandle) = 0;
        virtual void ClearRenderTarget(CommandHandle command_handle, TextureHandle render_target_handle,
                                       glm::vec4 clear_color) = 0;
        virtual void Draw(CommandHandle commandHandle,
                          u32 vertexCount,
                          u32 instanceCount,
                          u32 vertexOffset,
                          u32 firstInstance) = 0;
        virtual void DrawIndexed(CommandHandle commandHandle,
                                 u32 indexCount,
                                 u32 instanceCount,
                                 u32 firstIndex,
                                 int vertexOffset,
                                 u32 firstInstance) = 0;
        virtual void BlitToSwapchain(CommandHandle commandHandle, TextureHandle render_target_handle) = 0;

        [[nodiscard]] virtual TextureHandle CreateTexture(TextureCreateInfo create_info,
                                                          std::string_view debug_name) = 0;
        [[nodiscard]] virtual CommandHandle CreateCommand(QueueType queue_type, std::string_view debug_name) = 0;
        [[nodiscard]] virtual BufferHandle CreateBuffer(const BufferCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const GraphicsShaderCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const ComputeShaderCreateInfo& create_info,
                                                        std::string_view debug_name) = 0;

        virtual void DestroyTexture(TextureHandle render_target_handle) = 0;
        virtual void DestroyBuffer(BufferHandle buffer_handle) = 0;

        [[nodiscard]] virtual void* MapBuffer(BufferHandle buffer) = 0;
        virtual void UnmapBuffer(BufferHandle buffer) = 0;
        virtual u32 GetGPUAddress(TextureHandle texture_handle) = 0;
        virtual u32 GetGPUAddress(BufferHandle texture_handle) = 0;

        virtual void UploadToBuffer(BufferHandle buffer_handle, const BufferUploadInfo& info) = 0;

        FrameData& GetFrameData() { return m_frame_datas.at(m_frame_index); }

    protected:
        std::array<FrameData, kFrameCount> m_frame_datas = {};
        u32 m_frame_index = 0;
    };
}
