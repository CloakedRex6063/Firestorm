#pragma once

namespace FS
{
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
		
        [[nodiscard]] Renderer& Renderer() const { return *mRenderer;}
        [[nodiscard]] FileSystem& FileSystem() const { return *mFileSystem;}
        [[nodiscard]] ResourceSystem& ResourceSystem() const { return *mResourceSystem; }
        [[nodiscard]] Log& Log() const { return *mLog; }

        [[nodiscard]] float GetDeltaTime() const { return mDeltaTime; }
		
    private:
        std::shared_ptr<FS::Renderer> mRenderer;
        std::shared_ptr<FS::FileSystem> mFileSystem;
        std::shared_ptr<FS::ResourceSystem> mResourceSystem;
        std::shared_ptr<FS::Log> mLog;

        float mDeltaTime = 0.f;
    };

    extern Engine gEngine;

}  // namespace bee