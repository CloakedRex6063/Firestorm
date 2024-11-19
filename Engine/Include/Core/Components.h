#pragma once
#include "Systems/ResourceSystem.h"

namespace FS::Component
{
    struct Hierarchy
    {
        Entity mParent = NullEntity;
        std::vector<Entity> mChildren;
    };
    
    struct Name
    {
        std::string mName;
    };

    struct Transform
    {
        glm::vec3 mPosition;
        glm::vec3 mRotation;
        glm::vec3 mScale;
        glm::mat4 mWorldMatrix;
    };

    struct Renderable
    {
        ResourceHandle mResourceHandle;
    };
}