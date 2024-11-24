#include "Systems/CameraSystem.h"

#include "Core/ECS.h"
#include "Core/Engine.h"
#include "Core/Render/Renderer.h"

namespace FS
{
    void CameraSystem::Update(float deltaTime) {}

    glm::mat4 CameraSystem::GetViewMatrix() const
    {
        const auto& transform = gEngine.ECS().GetComponent<Component::Transform>(mCurrentCamera);
        return glm::inverse(transform.mWorldMatrix);
    }

    glm::mat4 CameraSystem::GetProjectionMatrix() const
    {
        const auto& camera = gEngine.ECS().GetComponent<Component::Camera>(mCurrentCamera);
        const glm::vec2 windowSize = gEngine.Renderer().GetWindow().GetSize();
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