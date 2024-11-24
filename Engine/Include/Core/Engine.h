#pragma once
#include "typeindex"

namespace FS
{
    class System;
    class ECS;
    class Renderer;
    class FileSystem;
    class ResourceSystem;
    class Log;
	
    class Engine
    {
    public:
        Engine();
        void BeginFrame() const;
        void Tick(float deltaTime);
        void EndFrame() const;
        ~Engine();

        [[nodiscard]] ECS& ECS() const { return *mECS; }
        [[nodiscard]] Renderer& Renderer() const { return *mRenderer;}
        [[nodiscard]] FileSystem& FileSystem() const { return *mFileSystem;}
        [[nodiscard]] ResourceSystem& ResourceSystem() const { return *mResourceSystem; }
        [[nodiscard]] Log& Log() const { return *mLog; }

        [[nodiscard]] float GetDeltaTime() const { return mDeltaTime; }

        template <typename SystemType>
        void AddSystem()
        {
            static_assert(std::is_base_of_v<System, SystemType>);
            mSystems[typeid(SystemType)] = std::move(std::make_unique<SystemType>());
        }
        
        template <typename SystemType>
        SystemType& GetSystem()
        {
            return *dynamic_cast<SystemType*>(mSystems[typeid(SystemType)].get());
        }

		
    private:
        std::shared_ptr<FS::ECS> mECS;
        std::shared_ptr<FS::Renderer> mRenderer;
        std::shared_ptr<FS::FileSystem> mFileSystem;
        std::shared_ptr<FS::ResourceSystem> mResourceSystem;
        std::shared_ptr<FS::Log> mLog;

        std::unordered_map<std::type_index, std::unique_ptr<System>> mSystems{};

        float mDeltaTime = 0.f;
    };

    extern Engine gEngine;

}  // namespace bee