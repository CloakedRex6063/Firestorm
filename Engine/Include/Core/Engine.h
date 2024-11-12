#pragma once

namespace FS
{
    class Renderer;
    class FileSystem;
    class ResourceSystem;
	
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
		
    private:
        std::shared_ptr<FS::Renderer> mRenderer;
        std::shared_ptr<FS::FileSystem> mFileSystem;
        std::shared_ptr<FS::ResourceSystem> mResourceSystem;
    };

    extern Engine gEngine;

}  // namespace bee