#pragma once

struct SDL_Window;
namespace FS
{
    struct WindowDeleter
    {
        void operator()(SDL_Window* window) const;
    };
    class Context
    {
    public:
        Context();
        [[nodiscard]] operator SDL_Window&() const { return *mWindow; };
        [[nodiscard]] glm::uvec2 GetWindowSize() const;
        
        [[nodiscard]] bool IsRunning() const {return mRunning;};
        [[nodiscard]] VkSurfaceKHR CreateSurface(const VkInstance& instance) const;
        [[nodiscard]] std::vector<const char*> RequiredExtensions() const;
        
        void PollEvents();
        static void LockMouse(bool toggle);

    private:
        std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
        bool mRunning = true;
    };
}