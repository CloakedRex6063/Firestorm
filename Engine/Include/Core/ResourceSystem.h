#pragma once
#include "Render/Resources/Model.hpp"

namespace FS
{
    struct ResourceHandle
    {
        std::string mPath;
    };
    
    class ResourceSystem
    {
    public:
        ResourceSystem();
        std::optional<Model> LoadModel(const std::string& filePath);
        [[nodiscard]] std::unordered_map<std::string, std::shared_ptr<Model>> GetModelsToUpload();
        [[nodiscard]] std::unordered_set<std::string> GetModelsToRelease();
        
    private:
        std::unordered_set<std::shared_ptr<std::string>> mModels;
        std::unordered_map<std::string, std::shared_ptr<Model>> mModelsToUpload;
    };
}