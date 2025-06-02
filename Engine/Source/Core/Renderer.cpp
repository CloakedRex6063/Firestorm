#include "Core/Renderer.hpp"
#include "Core/Engine.hpp"
#include "Core/Events.hpp"
#include "Core/FileIO.hpp"
#include "Core/Window.hpp"
#include "Render/DX12/RenderBackendDX12.hpp"

void FS::Renderer::Init()
{
    m_context = MakeScoped<RenderBackendDX12>();
    m_context->Init();

    CreateRenderTextures();
    CreateMeshBuffers();
    CreateTriangleShader();

    m_window_resize_listener = GEngine.Events().Subscribe<WindowResizeEvent>([this](const WindowResizeEvent&)
    {
        m_context->WaitForGPU();
        m_context->Resize();
        m_context->DestroyTexture(m_render_target);
        m_context->DestroyTexture(m_depth_stencil);
        CreateRenderTextures();
    });
}

void FS::Renderer::Update(float)
{
    auto& [command, render_target, fenceValue] = m_context->GetFrameData();

    m_context->BeginCommand(command);
    m_context->BindShader(command, m_triangle_shader);
    m_context->SetPrimitiveTopology(command, PrimitiveTopology::eTriangle);
    const Viewport viewport{.Dimensions = Window::GetWindowSize()};
    m_context->SetViewport(command, viewport);
    const Scissor scissor{.Max = Window::GetWindowSize()};
    m_context->SetScissor(command, scissor);

    const RenderPassInfo renderPassInfo{
        .RenderTargets = {render_target},
        .ClearColor = glm::vec4(0.392f, 0.584f, 0.929f, 1.0f),
    };
    m_context->BeginRenderPass(command, renderPassInfo);
    const struct PushConstant
    {
        u32 index = 0;
    } pc{
            .index = m_context->GetGPUAddress(m_vertex_buffer),
        };
    m_context->PushConstant(command, 1, &pc);

    m_context->Draw(command, 3, 1, 0, 0);

    m_context->EndRenderPass(command);

    m_context->EndCommand(command);

    m_context->Submit(Array{command}, QueueType::eGraphics);
    m_context->Present();
}

void FS::Renderer::Shutdown()
{
    GEngine.Events().Unsubscribe<WindowResizeEvent>(m_window_resize_listener);
    m_context->Shutdown();
}

void FS::Renderer::CreateMeshBuffers()
{
    constexpr std::array vertex_data{
        Vertex{.Position = glm::vec3(-0.5f, -0.5f, 0.0f)},
        Vertex{.Position = glm::vec3(0.5f, -0.5f, 0.0f)},
        Vertex{.Position = glm::vec3(0.0f, 0.5f, 0.0f)},
    };

    const BufferCreateInfo vertex_create_info
    {
        .FirstElement = 0,
        .NumElements = 10'000'000,
        .Stride = sizeof(Vertex),
        .Type = BufferType::eStorage,
        .UploadInfo = {
            .Data = vertex_data.data(),
            .Size = vertex_data.size() * sizeof(Vertex),
            .Offset = 0,
        },
    };
    m_vertex_buffer = m_context->CreateBuffer(vertex_create_info, "Vertex Buffer");

    constexpr BufferCreateInfo index_create_info
    {
        .FirstElement = 0,
        .NumElements = 15'000'000,
        .Stride = sizeof(u32),
    };
    m_index_buffer = m_context->CreateBuffer(index_create_info, "Index Buffer");
}

void FS::Renderer::CreateRenderTextures()
{
    const TextureCreateInfo create_info
    {
        .Dimensions = Window::GetWindowSize(),
        .Format = Format::eR16G16B16A16_FLOAT,
        .ViewType = ViewType::eTexture2D,
        .TextureFlags = TextureFlags::eRenderTexture,
    };
    m_render_target = m_context->CreateTexture(create_info, "Render Texture");

    const TextureCreateInfo depth_stencil_create_info
    {
        .Dimensions = Window::GetWindowSize(),
        .Format = Format::eD32_FLOAT,
        .ViewType = ViewType::eTexture2D,
        .TextureFlags = TextureFlags::eDepthTexture,
    };
    m_depth_stencil = m_context->CreateTexture(depth_stencil_create_info, "Depth Stencil");
}

void FS::Renderer::CreateTriangleShader()
{
    const GraphicsShaderCreateInfo shaderCreateInfo{
        .VertexCode = FileIO::ReadBinaryFile("Shaders/GeomVS.cso"),
        .FragmentCode = FileIO::ReadBinaryFile("Shaders/GeomPS.cso"),
        .PrimitiveTopology = PrimitiveTopology::eTriangle,
        .RenderTargetFormats = {Format::eB8G8R8A8_UNORM},
        .NumRenderTargets = 1,
        .DepthStencilFormat = Format::eUnknown,
    };
    m_triangle_shader = m_context->CreateShader(shaderCreateInfo, "Triangle Shader");
}
