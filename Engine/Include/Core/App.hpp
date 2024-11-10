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
}  // namespace bee

extern std::unique_ptr<FS::App> CreateApp();

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main()
{
    FS::gEngine.Init();
    const auto app = CreateApp();
    app->Init();
    
    while (FS::gEngine.Renderer().GetWindow().IsRunning())
    {
        FS::gEngine.BeginFrame();
        app->Update();
        FS::gEngine.Update();
        FS::gEngine.EndFrame();
    }
    app->Shutdown();
    FS::gEngine.Shutdown();
}

