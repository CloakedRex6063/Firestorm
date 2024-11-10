#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "Core/Render/Window.h"

namespace FS
{
    void WindowDeleter::operator()(SDL_Window* window) const
    {
        SDL_DestroyWindow(window);
    }
    
    Window::Window()
    {
        SDL_Init(SDL_INIT_VIDEO);
        const auto pointer = SDL_CreateWindow("Firestorm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        mWindow = std::unique_ptr<SDL_Window, WindowDeleter>(pointer);
    }

    bool Window::IsRunning() const
    {
        return mRunning;
    }

    void Window::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                mRunning = false;
            }
        }
    }

    std::vector<const char*> Window::GetRequiredExtensions() const
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, nullptr);
        std::vector<const char*> requiredExtensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, requiredExtensions.data());
        return requiredExtensions;
    }

    vk::raii::SurfaceKHR Window::CreateSurface(vk::raii::Instance& instance) const
    {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(mWindow.get(), *instance, &surface);
        //TODO: Log Error and exit
        return {instance, surface};
    }

    glm::uvec2 Window::GetSize() const
    {
        int width, height;
        SDL_GetWindowSize(mWindow.get(), &width, &height);
        return {width, height};
    }
}
