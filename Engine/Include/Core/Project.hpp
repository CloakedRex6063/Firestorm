#pragma once
#include "System.hpp"

namespace FS
{
    struct ProjectInfo
    {
        std::string Name;
        std::string Version;
    };

    struct ProjectData
    {
        ProjectInfo Info;
        std::string Path;
    };
    
    class Project
    {
    public:
        bool LoadProject(const std::filesystem::path& projectPath);
        static bool GenerateProject(const ProjectInfo& projectInfo, const std::filesystem::path& projectPath);
        [[nodiscard]] std::string_view GetProjectPath() const { return mProjectData.Path; }

    private:
        ProjectData mProjectData;
    };
}
