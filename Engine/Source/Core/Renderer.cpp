#include "Core/Renderer.hpp"
#include "Render/DX12/RenderBackendDX12.hpp"

void FS::Renderer::Init()
{
    m_context = MakeScoped<RenderBackendDX12>();
    m_context->Init();
}

void FS::Renderer::Update(float)
{
    const auto& [command, renderTarget, fenceValue] = m_context->GetFrameData();
    m_context->BeginCommand(command);
    m_context->ClearRenderTarget(command, renderTarget, {0.392, 0.584, 0.929, 1.0});
    m_context->EndCommand(command);
    m_context->Submit(std::array{command}, QueueType::eGraphics);
    m_context->Present();
}

void FS::Renderer::Shutdown()
{
    m_context->Shutdown();
}
