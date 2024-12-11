#pragma once
#include "System.hpp"

namespace FS
{
    class CameraSystem final : public System
    {
    public:
        void SetCurrentCamera(const Entity entity) { mCurrentCamera = entity; }
        void Update(float deltaTime) override;
        
        glm::vec3 GetPosition() const;
        void Move(glm::vec3 direction);
        void Rotate(float yaw, float pitch);
        
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
        [[nodiscard]] glm::mat4 GetProjectionMatrix() const;

    private:
        Entity mCurrentCamera = NullEntity;
        glm::mat4 mTranslation = glm::mat4(1.0f);
        glm::mat4 mRotation = glm::mat4(1.0f);
    };
}