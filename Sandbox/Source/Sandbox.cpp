#include "Core/App.hpp"
#include "Core/ECS.h"
#include "Core/FileSystem.h"
#include "Core/ResourceSystem.h"
#include "Systems/CameraSystem.h"

class Sandbox final : public FS::App
{
public:
    FS::ResourceHandle model;
    void Init() override
    {
        const auto cameraEntity = FS::gEngine.ECS().CreateEntity("Camera");
        FS::gEngine.ECS().AddComponent<FS::Component::Camera>(cameraEntity);
        FS::gEngine.ECS().AddComponent<FS::Component::Light>(cameraEntity).mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        FS::gEngine.GetSystem<FS::CameraSystem>().SetCurrentCamera(cameraEntity);
        auto& transform = FS::gEngine.ECS().GetComponent<FS::Component::Transform>(cameraEntity);
        transform.mWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,3.f));
        // transform.mWorldMatrix = glm::rotate(transform.mWorldMatrix, glm::radians(-90.f), glm::vec3(1, 0, 0));
        
    }
    void Update() override
    {
    }
    void Shutdown() override
    {
        
    }
};

std::unique_ptr<FS::App> CreateApp() { return std::make_unique<Sandbox>(); }