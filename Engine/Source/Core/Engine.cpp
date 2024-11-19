#include "Core/Engine.h"
#include "Core/Systems/FileSystem.h"
#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Renderer.h"
#include "Core/Systems/ResourceSystem.h"

namespace FS
{
    Engine gEngine;

    void Engine::Init()
    {
        mLog = std::make_shared<FS::Log>();
        mFileSystem = std::make_shared<FS::FileSystem>();
        mResourceSystem = std::make_shared<FS::ResourceSystem>();
        mRenderer = std::make_shared<VK::Renderer>();
        Log::Info("Initialised engine");
    }

    void Engine::BeginFrame() const { Renderer().BeginFrame(); }

    void Engine::Tick(float deltaTime) 
    {
        mDeltaTime = deltaTime;
        Renderer().Render();
    }

    void Engine::EndFrame() const { Renderer().EndFrame(); }

    void Engine::Shutdown() { Log::Info("Shutting down engine"); }
}  // namespace FS
