#pragma once
#include "System.hpp"

namespace FS
{
    class Renderer;
    class Window;

    class Engine
    {
    public:
        void Init();
        void Update(float dt);
        void Shutdown();

        [[nodiscard]] Window& Window() const { return *m_window; }
        [[nodiscard]] Renderer& Renderer() const { return *m_renderer; }

        void RequestQuit() { m_running = false; }
        [[nodiscard]] bool IsRunning() const { return m_running; }

        template <SystemConcept T>
        void AddSystem(const int priority = -1)
        {
            m_systems.push_back(new T());
            m_systems.back()->Init();
            m_systems.back()->m_priority = priority;
            std::ranges::sort(m_systems, [](const auto& a, const auto& b) { return a->m_priority < b->m_priority; });
        }

        template <SystemConcept T>
        constexpr T* GetSystem() const
        {
            auto it = std::ranges::find_if(m_systems, [](const auto& system) { return dynamic_cast<T*>(system); });
            if (it != m_systems.end())
            {
                return static_cast<T*>(*it);
            }
            return nullptr;
        }

    private:
        Vec<System*> m_systems;
        FS::Window* m_window = nullptr;
        FS::Renderer* m_renderer = nullptr;
        bool m_running = true;
        float m_delta_time = 0.0f;
    };

    inline Engine GEngine;
}
