#include "Core/FileSystem.h"

namespace FS
{
    FileSystem::FileSystem()
    {
        mPaths[Directory::eEngineAssets] = EngineAssets;
        mPaths[Directory::eEngineShaders] = EngineShaders;
        mPaths[Directory::eGameAssets] = GameAssets;
        mPaths[Directory::eGameConfig] = GameConfig;
    }

    std::vector<char> FileSystem::ReadTextFile(const Directory directory, const std::string& path)
    {
        const auto fullPath = GetPath(directory, path);
        std::ifstream file(fullPath);
        if (!file.is_open())
        {
            // Log::Error("File {} with full path {} was not found!", path, fullPath);
            return {};
        }
        file.seekg(0, std::ios::end);
        const std::streamsize size = file.tellg();
        std::vector<char> buffer(size);
        file.seekg(0);
        file.read(buffer.data(), size);
        return buffer;
    }

    std::vector<char> FileSystem::ReadBinaryFile(const Directory directory, const std::string& path)
    {
        const auto fullPath = GetPath(directory, path);
        std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            //Log::Error("File {} with full path {} was not found!", path, fullPath);
            return {};
        }
        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size)) return buffer;
        assert(false);
        return {};
    }

    std::string FileSystem::GetPath(const Directory directory, const std::string& path)
    {
        return mPaths[directory] + path;
    }
    
}
