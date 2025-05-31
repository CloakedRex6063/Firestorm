#pragma once
#include "chrono"
#include "Engine.hpp"

namespace FS
{
    class App
    {
    public:
        virtual ~App() = default;
        virtual void Init() = 0;
        virtual void Update(float dt) = 0;
        virtual void Shutdown() = 0;
    };
    Scoped<App> CreateApp();
}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main()
{
    FS::GEngine.Init();
    auto app = FS::CreateApp();
    app->Init();
    const auto prev = std::chrono::high_resolution_clock::now();
    while (FS::GEngine.IsRunning())
    {
        const auto now = std::chrono::high_resolution_clock::now();
        const auto dt = std::chrono::duration<float>(now - prev).count();
        FS::GEngine.Update(dt);
        app->Update(dt);
    }
    FS::GEngine.Shutdown();
    app->Shutdown();
}

