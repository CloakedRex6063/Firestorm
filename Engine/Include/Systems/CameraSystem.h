#pragma once
#include "System.hpp"

namespace FS
{
    class CameraSystem final : public System
    {
    public:
        void Update(float deltaTime) override;
        void SetCurrentCamera(const Entity entity) { mCurrentCamera = entity; }
        
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

    private:
        Entity mCurrentCamera = NullEntity;
    };
}