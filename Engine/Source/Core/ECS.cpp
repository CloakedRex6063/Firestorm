#include "Core/ECS.h"

namespace FS
{
    void ECS::EndFrame()
    {
        for (const auto& entity : mKillQueue)
        {
            mRegistry.destroy(entity);
        }
        mKillQueue.clear();

        for (const auto& [parent, child] : mRemoveChildren)
        {
            auto& children = GetComponent<Component::Hierarchy>(parent).mChildren;
            GetComponent<Component::Hierarchy>(child).mParent = entt::null;
            std::erase(children, child);
        }
        mRemoveChildren.clear();
    }

    Registry& ECS::GetRegistry() { return mRegistry; }

    Entity ECS::CreateEntity(const std::string& name)
    {
        auto& registry = GetRegistry();
        const auto entity = registry.create();
        AddComponent<Component::Name>(entity, name);
        AddComponent<Component::Transform>(entity, glm::vec3(), glm::vec3(), glm::vec3(1));
        AddComponent<Component::Hierarchy>(entity);
        return entity;
    }

    void ECS::DestroyEntity(Entity entity) { mKillQueue.emplace_back(entity); }

    bool ECS::IsValidEntity(const Entity entity) const { return mRegistry.valid(entity); }

    std::optional<Entity> ECS::GetParent(const Entity entity)
    {
        if (auto parentComponent = TryGetComponent<Component::Hierarchy>(entity))
        {
            return parentComponent.value().mParent;
        }
        return std::nullopt;
    }

    std::span<Entity> ECS::GetChildren(const Entity entity) { return GetComponent<Component::Hierarchy>(entity).mChildren; }

    bool ECS::AddChild(const Entity parent, Entity child)
    {
        GetComponent<Component::Hierarchy>(child).mParent = parent;
        GetComponent<Component::Hierarchy>(parent).mChildren.emplace_back(child);
        return true;
    }

    bool ECS::RemoveChild(const Entity parent, const Entity child)
    {
        mRemoveChildren[parent] = child;
        return true;
    }
}  // namespace FS