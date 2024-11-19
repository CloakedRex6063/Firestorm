#pragma once
#include "Core/Render/Resources/Model.hpp"
#include "fastgltf/core.hpp"

namespace FS
{
    class ResourceSystem
    {
    public:
        std::optional<ResourceHandle> LoadModel(const std::string& filePath);
        std::unordered_map<std::string, Model> GetModelsToUpload();
        std::unordered_set<std::string> GetModelsToRelease();
        
    private:
        std::optional<Model> LoadGLTF(const std::filesystem::path& filePath);
        static void ExtractGltfMeshes(Model& model, const fastgltf::Asset& asset);
        static void ExtractGltfMaterials(Model& model, const fastgltf::Asset& asset);
        static void ExtractGltfTextures(Model& model, const fastgltf::Asset& asset);
        static void ExtractGltfNodes(Model& model, const fastgltf::Asset& asset);

        std::unordered_map<std::string, Model> mModelsToUpload;
        std::unordered_set<ResourceHandle> mModels;
        fastgltf::Parser mParser;
    };
} // namespace FS