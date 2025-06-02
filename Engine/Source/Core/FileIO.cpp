#include "Core/FileIO.hpp"
#include "Core/Engine.hpp"
#include "Core/Project.hpp"
#include "Tools/Log.hpp"

namespace FS
{
    std::string FileIO::GetPath(const Location directory, const std::string_view path)
    {
        std::string fullPath;
        switch (directory)
        {
        case Location::eEngine:
            fullPath = path;
            break;
        case Location::eProject:
            fullPath = std::string(GEngine.Project().GetProjectPath()) + '/' + std::string(path);
            break;
        }
        return fullPath;
    }

    std::string FileIO::ReadTextFile(const std::string_view path)
    {

        const std::ifstream file(path.data(), std::ios::in);
        if (!file.is_open())
        {
            Log::Error("File {}  was not found!", path);
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    bool FileIO::WriteTextFile(const std::string_view path, const std::string_view content)
    {
        std::ofstream file(path.data());
        if (!file.is_open())
        {
            Log::Error("File {} was not found!", path);
            return false;
        }
        file << content;
        file.close();
        return true;
    }

    std::vector<char> FileIO::ReadBinaryFile(const std::string_view path)
    {
        std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            Log::Error("File {} was not found!", path);
            return {};
        }
        const int64_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size))
            return buffer;
        assert(false);
        return {};
    }

    bool FileIO::WriteBinaryFile(const std::string_view path, const std::vector<char>& content)
    {
        std::ofstream file(path.data(), std::ios::binary);
        if (!file.is_open())
        {
            Log::Error("File {} was not found!", path);
            return false;
        }
        file.write(content.data(), static_cast<int64_t>(content.size()));
        file.close();
        return true;
    }

    bool FileIO::Exists(const std::string_view path)
    {

        return !std::filesystem::exists(path);
    }

    uint64_t FileIO::LastModified(const std::string_view path)
    {

        const std::filesystem::file_time_type fTime = std::filesystem::last_write_time(path);
        return static_cast<uint64_t>(fTime.time_since_epoch().count());
    }
} // namespace FS
