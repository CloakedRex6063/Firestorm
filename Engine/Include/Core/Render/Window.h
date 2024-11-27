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

        [[nodiscard]] SDL_Window& GetSDLWindow() const {return *mWindow;}
        [[nodiscard]] bool IsRunning() const;
        [[nodiscard]] VkSurfaceKHR CreateSurface(const VkInstance& instance) const;
        [[nodiscard]] glm::uvec2 GetSize() const;
        [[nodiscard]] std::vector<const char*> RequiredExtensions() const;
        void PollEvents();

    private:
        std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
        bool mRunning = true;
    };
}
