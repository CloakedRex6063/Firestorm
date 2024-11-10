#pragma once

struct SDL_Window;

namespace FS
{
    struct WindowDeleter
    {
        void operator()(SDL_Window* window) const;
    };
    
    class Window
    {
    public:
        Window();

        [[nodiscard]] bool IsRunning() const;
        [[nodiscard]] vk::raii::SurfaceKHR CreateSurface(vk::raii::Instance& instance) const;
        [[nodiscard]] glm::uvec2 GetSize() const;
        [[nodiscard]] std::vector<const char*> GetRequiredExtensions() const;
        void PollEvents();

    private:
        std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
        bool mRunning = true;
    };
}
