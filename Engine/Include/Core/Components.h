#pragma once
#include "glm/fwd.hpp"
#include "Core/Render/Resources/Model.hpp"
#include "Tools/Enums.h"

namespace FS::Component
{
    struct Hierarchy
    {
        Entity mParent = NullEntity;
        std::vector<Entity> mChildren{};
    };
    
    struct Name
    {
        std::string mName;
    };

    struct Transform
    {
        glm::vec3 mPosition{};
        glm::vec3 mRotation{};
        glm::vec3 mScale{};
        glm::mat4 mWorldMatrix = glm::mat4(1.0f);

        glm::vec3 GetWorldPosition() { return glm::vec3(mWorldMatrix[3]); }
        glm::vec3 GetWorldRotation();
        glm::vec3 GetWorldScale();
    };

    struct Renderable
    {
        ResourceHandle mResourceHandle;
    };
    
    struct Light
    {
        glm::vec3 mPosition = glm::vec3(0.0f);
        LightType mType = LightType::ePoint;
        glm::vec3 mColor = glm::vec3(1.0f);
        float mIntensity = 1.0f;
        glm::vec3 mDirection = glm::vec3(0.0f);
    };

    struct Camera
    {
        CameraType mType = CameraType::ePerspective;
        float mNear = 0.01f;
        float mFar = 1000.f;
        float mFov = 90.f;
        glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 mViewMatrix = glm::mat4(1.0f);
    };
}