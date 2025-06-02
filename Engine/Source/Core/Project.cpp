#include "Core/Project.hpp"
#include "glaze/glaze.hpp"

namespace FS
{
    bool Project::LoadProject(const std::filesystem::path& projectPath)
    {
        const auto ec = glz::read_file_json(mProjectData.Info, projectPath.generic_string(), std::string{}).ec;
        if (ec != glz::error_code::none)
        {
            Log::Error("Failed to load project");
            return false;
        }

        mProjectData = ProjectData
        {
            .Path = projectPath.parent_path().string(),
        };
        Log::Info("Loaded project: {}", mProjectData.Path);
        return true;
    }

    bool Project::GenerateProject(const ProjectInfo& projectInfo, const std::filesystem::path& projectPath)
    {
        if (!std::filesystem::exists(projectPath))
        {
            std::filesystem::create_directory(projectPath);
        }

        const std::string filePath = projectPath.string() + "/" + projectInfo.Name + ".proj";

        const auto result = glz::write_file_json(projectInfo, filePath, std::string{});
        return result.ec == glz::error_code::none;
    }
}

