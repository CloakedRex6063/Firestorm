/*#include "Core/Systems/ECS.h"

namespace FS
{
    std::shared_ptr<Registry> ECS::CreateRegistry()
    {
        auto registry = std::make_shared<Registry>();
        //TODO: handle reflection
        return registry;
    }
    
    Registry& ECS::GetRegistry()
    {
        
    }
    
    Entity ECS::CreateEntity(const std::string& name)
    {
        auto& registry = GetRegistry();
        const auto entity = registry.create();
        AddComponent<Component::Name>(entity, name);
        AddComponent<Component::Transform>(entity, glm::vec3(), glm::vec3(), glm::vec3(1));
        AddComponent<Component::Hierarchy>(entity);
        return entity;
    }
    
    void ECS::DestroyEntity(Entity entity)
    {
        
    }
    
    bool ECS::IsValidEntity(Entity entity)
    {
        
    }
    
    std::optional<Entity> ECS::GetParent(Entity entity)
    {
        
    }
    std::span<Entity>& ECS::GetChildren(Entity entity) {}
    bool ECS::AddChild(Entity parent, Entity child) {}
    bool ECS::RemoveChild(Entity parent, Entity child) {}
}  // namespace FS*/