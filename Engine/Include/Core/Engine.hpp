#pragma once
#include "Core/System.hpp"

namespace FS
{
    class Renderer;
    class Window;
    class Events;
    class Project;

    class Engine
    {
    public:
        void Init();
        void Update(float dt);
        void Shutdown();

        [[nodiscard]] Window& Window() const { return *m_window; }
        [[nodiscard]] Renderer& Renderer() const { return *m_renderer; }
        [[nodiscard]] Project& Project() const { return *m_project; }
        [[nodiscard]] Events& Events() const { return *m_events; } 

        void RequestQuit() { m_running = false; }
        [[nodiscard]] bool IsRunning() const { return m_running; }

        template <SystemConcept T>
        void AddSystem(const int priority = -1)
        {
            m_systems[Hash<T>()] = MakeRef<T>();
            auto& system = m_systems[Hash<T>()];
            m_system_order.push_back(Hash<T>()); 
            system->Init();
            system->m_priority = priority;
        }

        template<typename Func>
        void IterateSystems(Func&& callback)
        {
            for (auto hash : m_system_order)
            {
                callback(m_systems[hash]);
            }
        };

        template <SystemConcept T>
        constexpr T* GetSystem() const
        {
            return static_cast<T*>(m_systems[Hash<T>()].get());
        }

    private:
        std::unordered_map<TypeHash, Ref<System>> m_systems;
        std::vector<TypeHash> m_system_order;
        Ref<FS::Window> m_window = nullptr;
        Ref<FS::Renderer> m_renderer = nullptr;
        Ref<FS::Project> m_project = nullptr;
        Ref<FS::Events> m_events = nullptr;
        bool m_running = true;
        float m_delta_time = 0.0f;
    };

    inline Engine GEngine;
}
