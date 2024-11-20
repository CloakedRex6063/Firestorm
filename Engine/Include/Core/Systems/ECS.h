// #pragma once
// #include "Systems/System.hpp"
//
// namespace FS
// {
//     class ECS
//     {
//     public:
//         std::shared_ptr<Registry> CreateRegistry();
//         Registry& GetRegistry();
//
//         Entity CreateEntity(const std::string& name);
//         void DestroyEntity(Entity entity);
//         bool IsValidEntity(Entity entity);
//
//         std::optional<Entity> GetParent(Entity entity);
//         std::span<Entity>& GetChildren(Entity entity);
//         bool AddChild(Entity parent, Entity child);
//         bool RemoveChild(Entity parent, Entity child);
//
//         template <class Component, class... Args>
//         Component& AddComponent(Entity entity, Args&&... args);
//         template <class... Component>
//         void RemoveComponents(Entity entity);
//         template <class... Component>
//         bool HasComponents(Entity entity);
//         template <class... Component>
//         bool HasAnyComponent(Entity entity);
//         template <class... Component>
//         std::tuple<Component...> GetComponent(Entity entity);
//         template <class Component>
//         std::optional<Component> TryGetComponent(Entity entity);
//
//     private:
//         std::unordered_set<System> mSystems;
//     };
//     template <class Component, class... Args>
//     Component& ECS::AddComponent(Entity entity, Args&&... args)
//     {
//         return GetRegistry().emplace<Component>(entity, std::forward<Args>(args)...);
//     }
//     template <class... Component>
//     void ECS::RemoveComponents(const Entity entity)
//     {
//         GetRegistry().remove<Component...>(entity);
//     }
//     template <class... Component>
//     bool ECS::HasComponents(const Entity entity)
//     {
//         return GetRegistry().all_of<Component...>(entity);
//     }
//     template <class... Component>
//     bool ECS::HasAnyComponent(const Entity entity)
//     {
//         return GetRegistry().any_of<Component...>(entity);
//     }
//     template <class... Component>
//     std::tuple<Component...> ECS::GetComponent(const Entity entity)
//     {
//         return GetRegistry().get<Component...>(entity);
//     }
//     template <class Component>
//     std::optional<Component> ECS::TryGetComponent(const Entity entity)
//     {
//         auto component = GetRegistry().try_get<Component>(entity);
//         return component ? std::optional<Component>(*component) : std::nullopt;
//     }
// }  // namespace FS