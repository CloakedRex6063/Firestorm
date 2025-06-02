#pragma once 
#include "Events.hpp"
#include "System.hpp"
#include "Render/IRenderBackend.hpp"

namespace FS
{
    class Renderer final : public System
    {
    public:
        void Init() override;
        void Update(float) override;
        void Shutdown() override;

    private:
        void CreateMeshBuffers();
        void CreateRenderTextures();
        void CreateTriangleShader();
        
        ListenerHandle m_window_resize_listener = ListenerHandle::eNull;
        Scoped<IRenderBackend> m_context;

        TextureHandle m_render_target = TextureHandle::eNull;
        TextureHandle m_depth_stencil = TextureHandle::eNull;
        
        BufferHandle m_vertex_buffer = BufferHandle::eNull;
        BufferHandle m_index_buffer = BufferHandle::eNull;
        
        ShaderHandle m_triangle_shader = ShaderHandle::eNull;
    };
}
