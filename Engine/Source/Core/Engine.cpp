#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "Core/ECS.h"
#include "Core/ResourceSystem.h"
#include "Systems/CameraSystem.h"
#include "Tools/Log.h"

namespace FS
{
    Engine gEngine;
    
    Engine::Engine()
    {
        mLog = std::make_shared<FS::Log>();
        mFileSystem = std::make_shared<FS::FileSystem>();
        mECS = std::make_shared<FS::ECS>();
        mResourceSystem = std::make_shared<FS::ResourceSystem>();
        mRenderer = std::make_shared<VulkanRenderer>();

        gEngine.AddSystem<CameraSystem>();
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

    void Engine::EndFrame() const
    {
        Renderer().EndFrame();
        ECS().EndFrame();
    }
}  // namespace FS
