#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "Core/Render/Window.h"

#include <backends/imgui_impl_sdl2.h>

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

    bool Window::IsRunning() const { return mRunning; }

    VkSurfaceKHR Window::CreateSurface(const VkInstance& instance) const
    {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(mWindow.get(), instance, &surface))
        {
            Log::Critical("Failed to create Vulkan surface!, exiting....");
            exit(-1);
        }
        return surface;
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
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
    }

    std::vector<const char*> Window::RequiredExtensions() const
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, nullptr);
        std::vector<const char*> requiredExtensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, requiredExtensions.data());
        return requiredExtensions;
    }

    glm::uvec2 Window::GetSize() const
    {
        int width, height;
        SDL_GetWindowSize(mWindow.get(), &width, &height);
        return {width, height};
    }
}
