#include "Core/Engine.h"
#include "Core/Context.h"
#include "Core/FileIO.h"
#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "Core/ECS.h"
#include "Core/Input.h"
#include "Core/ResourceManager.h"
#include "Systems/CameraSystem.h"
#include "Tools/Log.h"

namespace FS
{
    Engine gEngine;
    
    Engine::Engine()
    {
        mLog = std::make_shared<FS::Log>();
        mContext = std::make_shared<FS::Context>();
        mInput = std::make_shared<FS::Input>();
        mFileIO = std::make_shared<FS::FileIO>();
        mECS = std::make_shared<FS::ECS>();
        mResourceManager = std::make_shared<FS::ResourceManager>();
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
        for (auto& system : std::views::values(mSystems))
        {
            system->Update(deltaTime);
        }
    }

    void Engine::EndFrame() const
    {
        Renderer().EndFrame();
        ECS().EndFrame();
    }
}  // namespace FS
