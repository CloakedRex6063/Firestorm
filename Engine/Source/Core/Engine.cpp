#include "Core/Engine.h"
#include "Core/Systems/FileSystem.h"
#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "Core/Systems/ResourceSystem.h"

namespace FS
{
    Engine gEngine;
    
    Engine::Engine()
    {
        mLog = std::make_shared<FS::Log>();
        mFileSystem = std::make_shared<FS::FileSystem>();
        mResourceSystem = std::make_shared<FS::ResourceSystem>();
        mRenderer = std::make_shared<VulkanRenderer>();
        Log::Info("Initialised engine");
    }

    Engine::~Engine()
    {
        Log::Info("Shutting down engine");
    }

    void Engine::BeginFrame() const { Renderer().BeginFrame(); }

    void Engine::Tick(float deltaTime) 
    {
        mDeltaTime = deltaTime;
        Renderer().Render();
    }

    void Engine::EndFrame() const { Renderer().EndFrame(); }
}  // namespace FS
