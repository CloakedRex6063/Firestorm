#include "Core/Engine.h"

#include "Core/FileSystem.h"
#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Renderer.h"
#include "Core/ResourceSystem.h"

namespace FS
{
    Engine FS::gEngine; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    void Engine::Init()
    {
        mFileSystem = std::make_shared<FS::FileSystem>();
        mRenderer = std::make_shared<VK::Renderer>();
        mResourceSystem = std::make_shared<FS::ResourceSystem>();
    }

    void Engine::BeginFrame() const { Renderer().BeginFrame(); }

    void Engine::EndFrame() const { Renderer().EndFrame(); }

    void Engine::Update() const { Renderer().Render(); }

    void Engine::Shutdown() {}
} // namespace FS
