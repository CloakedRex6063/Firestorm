#pragma once

namespace FS
{
    class Renderer;
    class FileSystem;
	
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
		
    private:
        std::shared_ptr<FS::Renderer> mRenderer;
        std::shared_ptr<FS::FileSystem> mFileSystem;
    };

    extern Engine gEngine;

}  // namespace bee