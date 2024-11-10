#pragma once

namespace FS
{
    class Window;
    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer() = default;
        virtual void BeginFrame() = 0;
        virtual void Render() = 0;
        virtual void EndFrame() = 0;

        [[nodiscard]] Window& GetWindow() const { return *mWindow; }
        
    private:
        std::unique_ptr<Window> mWindow;
    };
}
