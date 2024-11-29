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
        
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
        [[nodiscard]] glm::mat4 GetProjectionMatrix() const;

    private:
        Entity mCurrentCamera = NullEntity;
    };
}