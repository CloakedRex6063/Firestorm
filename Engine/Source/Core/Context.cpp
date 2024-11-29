#include "Core/Context.h"

#include "Core/Engine.h"
#include "Core/Input.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "backends/imgui_impl_sdl2.h"

namespace FS
{
    void WindowDeleter::operator()(SDL_Window* window) const
    {
        SDL_DestroyWindow(window);
    }
    Context::Context()
    {
        SDL_Init(SDL_INIT_VIDEO);
        const auto pointer = SDL_CreateWindow("Firestorm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        mWindow = std::unique_ptr<SDL_Window, WindowDeleter>(pointer);
    }
    
    VkSurfaceKHR Context::CreateSurface(const VkInstance& instance) const
    {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(mWindow.get(), instance, &surface))
        {
            Log::Critical("Failed to create Vulkan surface!, exiting....");
            exit(-1);
        }
        return surface;
    }
    
    glm::uvec2 Context::GetWindowSize() const
    {
        int width, height;
        SDL_GetWindowSize(mWindow.get(), &width, &height);
        return {width, height};
    }
    
    std::vector<const char*> Context::RequiredExtensions() const
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, nullptr);
        std::vector<const char*> requiredExtensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(mWindow.get(), &extensionCount, requiredExtensions.data());
        return requiredExtensions;
    }
    void Context::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    mRunning = false;
                    break;
                case SDL_KEYDOWN:
                    gEngine.Input().mKeysDown[static_cast<Input::KeyboardKey>(event.key.keysym.sym)] = true;
                    break;
                case SDL_KEYUP:
                    gEngine.Input().mKeysDown[static_cast<Input::KeyboardKey>(event.key.keysym.sym)] = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    gEngine.Input().mButtonsDown[static_cast<Input::MouseButton>(event.button.button)] = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    gEngine.Input().mButtonsDown[static_cast<Input::MouseButton>(event.button.button)] = false;
                    break;
                case SDL_MOUSEMOTION:
                    gEngine.Input().mMousePosition = glm::vec2(event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    gEngine.Input().mMouseWheel = glm::vec2(event.wheel.x, -event.wheel.y);
                default:
                    break;
            }
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
    }
    
    void Context::LockMouse(const bool toggle)
    {
        SDL_ShowCursor(!toggle);
    }

}  // namespace FS