#include "Core/Engine.h"

#include "Core/FileSystem.h"
#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Renderer.h"
#include "Core/ResourceSystem.h"

namespace FS
{
    Engine gEngine;

    void Engine::Init()
    {
        mLog = std::make_shared<FS::Log>();
        mFileSystem = std::make_shared<FS::FileSystem>();
        mRenderer = std::make_shared<VK::Renderer>();
        mResourceSystem = std::make_shared<FS::ResourceSystem>();
        Log::Info("Initialised engine");
    }

    void Engine::BeginFrame() const { Renderer().BeginFrame(); }

    void Engine::Update() const { Renderer().Render(); }
    
    void Engine::EndFrame() const { Renderer().EndFrame(); }

    void Engine::Shutdown() { Log::Info("Shutting down engine"); }
} // namespace FS
