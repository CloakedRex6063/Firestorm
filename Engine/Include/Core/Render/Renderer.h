#pragma once
#include "Core/Render/Window.h"

namespace FS
{
    class Renderer
    {
    public:
        Renderer() : mWindow(std::make_unique<Window>()) {}
        virtual ~Renderer() = default;
        virtual void BeginFrame() = 0;
        virtual void Render() = 0;
        virtual void EndFrame() = 0;

        [[nodiscard]] Window& GetWindow() const { return *mWindow; }
        
    private:
        std::unique_ptr<Window> mWindow;
    };
}
