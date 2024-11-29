#pragma once

namespace FS
{
    enum class Directory : std::uint8_t
    {
        eEngineAssets,
        eEngineShaders,
        eGameConfig,
        eGameAssets,
        eNone
    };
    
    class FileIO
    {
    public:
        FileIO();
        std::vector<char> ReadTextFile(Directory directory, const std::string& path);
        std::vector<char> ReadBinaryFile(Directory directory, const std::string& path);
        std::string GetPath(Directory directory, const std::string& path);

    private:
        std::unordered_map<Directory, std::string> mPaths;
    };
};