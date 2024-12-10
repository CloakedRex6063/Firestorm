#include "Core/App.hpp"
#include "Core/ECS.h"
#include "Core/FileIO.h"
#include "Core/Input.h"
#include "Core/ResourceManager.h"
#include "Systems/CameraSystem.h"

class Sandbox final : public FS::App
{
public:
    void Init() override
    {
        const auto cameraEntity = FS::gEngine.ECS().CreateEntity("Camera");
        FS::gEngine.ECS().AddComponent<FS::Component::Camera>(cameraEntity);
        auto& light = FS::gEngine.ECS().AddComponent<FS::Component::Light>(cameraEntity);
        light.mColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
        light.mPosition = glm::vec3(0, 1, 1);
        light.mDirection = glm::vec3(0, 0, -1);
        light.mIntensity = 1.f;
        light.mType = FS::LightType::eDirectional;
        
        FS::gEngine.GetSystem<FS::CameraSystem>().SetCurrentCamera(cameraEntity);
        auto& transform = FS::gEngine.ECS().GetComponent<FS::Component::Transform>(cameraEntity);
        transform.mPosition = glm::vec3(0, 0, 3);
        transform.mWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,3.f));

        const auto otherModelPath = FS::gEngine.FileIO().GetPath(FS::Directory::eGameAssets, "Models/Helmet/DamagedHelmet.gltf");
        auto otherModel = FS::gEngine.ResourceManager().LoadModel(otherModelPath).value();

        // const auto modelPath = FS::gEngine.FileIO().GetPath(FS::Directory::eGameAssets, "Models/DamagedHelmet.glb");
        // auto model = FS::gEngine.ResourceManager().LoadModel(modelPath).value();
    }
    
    void Update(float deltaTime) override
    {
        auto direction = glm::vec3(0, 0, 0);
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::W))
        {
            direction = glm::vec3(0, 0, -1);
        }
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::S))
        {
            direction = glm::vec3(0, 0, 1);
        }
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::A))
        {
            direction = glm::vec3(-1, 0, 0);
        }
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::D))
        {
            direction = glm::vec3(1, 0, 0);
        }
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::Space))
        {
            direction = glm::vec3(0, 1, 0);
        }
        if (FS::gEngine.Input().GetKeyboardKey(FS::Input::KeyboardKey::LeftControl))
        {
            direction = glm::vec3(0, -1, 0);
        }
        if (FS::gEngine.Input().GetMouseButton(FS::Input::MouseButton::Right))
        {
            FS::Context::LockMouse(true);
        }
        else
        {
            FS::Context::LockMouse(false);
        }
        FS::gEngine.GetSystem<FS::CameraSystem>().Move(direction * 5.f * deltaTime);
    }
    void Shutdown() override
    {
        
    }
};

std::unique_ptr<FS::App> CreateApp() { return std::make_unique<Sandbox>(); }