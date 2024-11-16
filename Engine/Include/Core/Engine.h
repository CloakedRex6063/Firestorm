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
        void Init();
        void BeginFrame() const;
        void Update() const;
        void EndFrame() const;
        void Shutdown();
		
        [[nodiscard]] Renderer& Renderer() const { return *mRenderer;}
        [[nodiscard]] FileSystem& FileSystem() const { return *mFileSystem;}
        [[nodiscard]] ResourceSystem& ResourceSystem() const { return *mResourceSystem; }
        [[nodiscard]] Log& Log() const { return *mLog; }
		
    private:
        std::shared_ptr<FS::Renderer> mRenderer;
        std::shared_ptr<FS::FileSystem> mFileSystem;
        std::shared_ptr<FS::ResourceSystem> mResourceSystem;
        std::shared_ptr<FS::Log> mLog;
    };

    extern Engine gEngine;

}  // namespace bee