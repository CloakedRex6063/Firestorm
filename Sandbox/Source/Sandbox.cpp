#include "Core/App.hpp"
#include "Core/ECS.h"
#include "Core/FileIO.h"
#include "Core/Input.h"
#include "Core/ResourceManager.h"
#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "Systems/CameraSystem.h"

class Sandbox final : public FS::App
{
public:
    void Init() override
    {
        const auto cameraEntity = FS::gEngine.ECS().CreateEntity("Camera");
        FS::gEngine.ECS().AddComponent<FS::Component::Camera>(cameraEntity);
        
        const auto entity = FS::gEngine.ECS().CreateEntity("Light");
        auto& [mPosition, mType, mColor, mIntensity, mDirection] = FS::gEngine.ECS().AddComponent<FS::Component::Light>(entity);
        mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        mPosition = glm::vec3(0, 1, 1);
        mDirection = glm::vec3(0, 0, -1);
        mIntensity = 2.f;
        mType = FS::LightType::ePoint;
        
        FS::gEngine.GetSystem<FS::CameraSystem>().SetCurrentCamera(cameraEntity);
        FS::gEngine.GetSystem<FS::CameraSystem>().Move(glm::vec3(0, 0, 1));

        // const auto otherModelPath = FS::gEngine.FileIO().GetPath(FS::Directory::eGameAssets, "Models/Helmet/DamagedHelmet.gltf");
        // auto otherModel = FS::gEngine.ResourceManager().LoadModel(otherModelPath).value();

        const auto modelPath = FS::gEngine.FileIO().GetPath(FS::Directory::eGameAssets, "Models/chess/ABeautifulGame.gltf");
        auto model = FS::gEngine.ResourceManager().LoadModel(modelPath).value();
    }
    
    void Update(const float deltaTime) override
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
        
        static float pitch = 0, yaw = 0;
        static auto previousMousePos = FS::gEngine.Input().GetMousePosition();
        static bool firstMouse = true;
        if (FS::gEngine.Input().GetMouseButton(FS::Input::MouseButton::Right))
        {
            if (firstMouse)
            {
                firstMouse = false;
                previousMousePos = FS::gEngine.Input().GetMousePosition();
            }
            const auto currentMousePos = FS::gEngine.Input().GetMousePosition();
            pitch -= (currentMousePos.y - previousMousePos.y) * deltaTime * 5.f;
            yaw -= (currentMousePos.x - previousMousePos.x) * deltaTime * 5.f;
            previousMousePos = currentMousePos;
        }
        else
        {
            firstMouse = true;
        }
        
        FS::gEngine.GetSystem<FS::CameraSystem>().Rotate(yaw, pitch);
        FS::gEngine.GetSystem<FS::CameraSystem>().Move(direction * 1.f * deltaTime);
    }
    void Shutdown() override
    {
        
    }
};

std::unique_ptr<FS::App> CreateApp() { return std::make_unique<Sandbox>(); }