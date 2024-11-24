#pragma once
#include "Engine.h"
#include "Render/Renderer.h"
#include "Render/Window.h"

// Include this file only once in your application.

namespace FS
{
    class App
    {
    public:
        virtual ~App() = default;
        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Shutdown() = 0;
    };
} // namespace FS

extern std::unique_ptr<FS::App> CreateApp();

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main()
{
    const auto app = CreateApp();
    app->Init();
    FS::Log::Info("Initialized app");
    
    std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    while (FS::gEngine.Renderer().GetWindow().IsRunning())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        FS::gEngine.BeginFrame();
        app->Update();
        FS::gEngine.Tick(deltaTime.count());
        FS::gEngine.EndFrame();
    }
    FS::Log::Info("Shutting down app");
    app->Shutdown();
}
