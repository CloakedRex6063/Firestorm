#include "Core/Engine.hpp"
#include "Core/Window.hpp"
#include "Core/Renderer.hpp"
#include "Core/Project.hpp"
#include "Core/Events.hpp"
#include "Tools/Log.hpp"

void FS::Engine::Init()
{
    m_events = MakeRef<FS::Events>();
    m_project = MakeRef<FS::Project>();
    
    AddSystem<FS::Window>();
    AddSystem<FS::Renderer>();

    Log::Info("Core Systems Initialized");
}

void FS::Engine::Update(const float dt)
{
    m_delta_time = dt;
    IterateSystems([dt](const Ref<System>& system)
    {
        system->Update(dt);
    });
}

void FS::Engine::Shutdown()
{
    IterateSystems([](const Ref<System>& system)
    {
        system->Shutdown();
    });
    m_systems.clear();
    Log::Info("Engine Shutdown");
}
