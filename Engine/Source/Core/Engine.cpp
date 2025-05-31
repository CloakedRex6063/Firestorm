#include "Core/Engine.hpp"
#include "Core/Window.hpp"
#include "Core/Renderer.hpp"
#include "Tools/Log.hpp"

void FS::Engine::Init()
{
    AddSystem<FS::Window>();
    AddSystem<FS::Renderer>();
    Log::Info("Core Systems Initialized");
}

void FS::Engine::Update(const float dt)
{
    m_delta_time = dt;
    for (const auto& system : m_systems)
    {
        system->Update(dt);
    }
}

void FS::Engine::Shutdown()
{
    for (const auto& system : m_systems)
    {
        system->Shutdown();
    }
    m_systems.clear();
    Log::Info("Engine Shutdown");
}
