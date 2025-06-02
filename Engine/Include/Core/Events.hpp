#pragma once
#include "Tools/Tools.hpp"

namespace FS
{
    enum class ListenerHandle : u32
    {
        eNull,
    };
    class Events
    {
    public:
        template <typename Event>
        ListenerHandle Subscribe(std::function<void(const Event&)> callback)
        {
            Log::Debug("Subscribed to event: {}", typeid(Event).name());
            auto& listener = m_listeners[Hash<Event>()];
            const auto number = RandomNumber(0, std::numeric_limits<int>::max());
            const auto handle = static_cast<ListenerHandle>(number);
            auto call = [callback](const void* e)
            {
                callback(*static_cast<const Event*>(e));
            };
            listener.callbacks[handle] = call;
            return handle;
        }

        template<typename Event>
        bool Unsubscribe(ListenerHandle listener_handle)
        {
            Log::Debug("Unsubscribed from event: {}", typeid(Event).name());
            auto& listener = m_listeners[Hash<Event>()];
            if (listener.callbacks.contains(listener_handle))
            {
                listener.callbacks.erase(listener_handle);
                return true;
            }
            return false;
        }
        
        template <typename Event>
        void Broadcast(const Event& event)
        {
            if (m_listeners.contains(Hash<Event>()))
            {
                for (const auto& callback : std::views::values(m_listeners[Hash<Event>()].callbacks))
                {
                    callback(&event);   
                }
            }
        }

    private:
        struct Listener
        {
            std::unordered_map<ListenerHandle, std::function<void(const void*)>> callbacks;
        };
        std::unordered_map<TypeHash, Listener> m_listeners;
    };
}
