#pragma once 
#include "System.hpp"

namespace FS
{
    struct WindowResizeEvent
    {
        glm::uvec2 Size;
    };
    class Window final : public System
    {
    public:
        void Init() override;
        void Update(float) override;
        void Shutdown() override;

        static glm::vec2 GetWindowSize();
        static void* GetHandle();
    };
}
