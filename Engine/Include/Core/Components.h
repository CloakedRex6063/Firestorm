#pragma once
#include "Core/Render/Resources/Model.hpp"
#include "Tools/Enums.h"

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
        glm::mat4 mWorldMatrix = glm::mat4(1.0f);
    };

    struct Renderable
    {
        ResourceHandle mResourceHandle;
    };

    struct Camera
    {
        CameraType mType = CameraType::ePerspective;
        float mNear = 0.01f;
        float mFar = 1000.f;
        float mFov = 90.f;
        glm::mat4 mProjectionMatrix;
        glm::mat4 mViewMatrix;
    };
}