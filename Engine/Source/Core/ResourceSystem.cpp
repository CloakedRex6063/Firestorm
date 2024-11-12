#include "Core/ResourceSystem.h"

namespace FS
{
    ResourceSystem::ResourceSystem()
    {
        mModels.emplace(std::make_shared<std::string>(""));
    }
    
    std::optional<Model> ResourceSystem::LoadModel(const std::string& filePath)
    {
        return {};
    }
    
    std::unordered_map<std::string, std::shared_ptr<Model>> ResourceSystem::GetModelsToUpload()
    {
        std::unordered_map<std::string, std::shared_ptr<Model>> newModels = std::move(mModelsToUpload);
        mModelsToUpload.clear();
        return newModels;
    }
    
    std::unordered_set<std::string> ResourceSystem::GetModelsToRelease() 
    {
        std::unordered_set<std::string> modelsToRelease;

        for (auto it = mModels.begin(); it != mModels.end(); ) 
        {
            if (it->use_count() == 1)
            {
                // Dereference shared_ptr to get the string and add it directly
                modelsToRelease.emplace(**it); 
                it = mModels.erase(it);  // Erase and move iterator to next element
            }
            else
            {
                ++it;
            }
        }

        return modelsToRelease;
    }
} // namespace FS