#include "Systems/CameraSystem.h"
#include "Core/Components.h"
#include "Core/Context.h"
#include "Core/ECS.h"
#include "Core/Engine.h"
#include "glm/gtx/euler_angles.hpp"

namespace FS
{
    void CameraSystem::Update(float deltaTime)
    {
        auto& transform = gEngine.ECS().GetComponent<Component::Transform>(mCurrentCamera);
        transform.mWorldMatrix = mTranslation * mRotation;
    }

    void CameraSystem::Move(const glm::vec3 direction) 
    {
        auto& transform = gEngine.ECS().GetComponent<Component::Transform>(mCurrentCamera);
        mTranslation = glm::translate(glm::mat4(1.0f), transform.mPosition += direction);
    }
    
    void CameraSystem::Rotate(const float yaw, const float pitch) 
    {
        auto clampedPitch = glm::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>()); 
        mRotation = glm::yawPitchRoll(yaw, clampedPitch, 0.0f);
    }

    glm::vec3 CameraSystem::GetPosition() const
    {
        auto& transform = gEngine.ECS().GetComponent<Component::Transform>(mCurrentCamera);
        return {transform.mWorldMatrix[3]};
    }

    glm::mat4 CameraSystem::GetViewMatrix() const
    {
        const auto& transform = gEngine.ECS().GetComponent<Component::Transform>(mCurrentCamera);
        return glm::inverse(transform.mWorldMatrix);
    }

    glm::mat4 CameraSystem::GetProjectionMatrix() const
    {
        const auto& camera = gEngine.ECS().GetComponent<Component::Camera>(mCurrentCamera);
        const glm::vec2 windowSize = gEngine.Context().GetWindowSize();
        const auto aspect = windowSize.x / windowSize.y;
        if (camera.mType == CameraType::eOrthographic)
        {
            constexpr float orthoSize = 1.0f;
            return glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, camera.mNear, camera.mFar);
        }
        auto projection = glm::perspective(glm::radians(camera.mFov), aspect, camera.mNear, camera.mFar);
        projection[1][1] *= -1;
        return projection;
    }
}  // namespace FS