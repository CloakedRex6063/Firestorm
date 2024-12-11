#pragma once
#include "Render/Resources/Model.hpp"

namespace fastgltf
{
    class Asset;
}

namespace FS
{
    class ResourceManager
    {
    public:
        ResourceManager();
        std::optional<ResourceHandle> LoadModel(const std::string& modelPath);
        std::unordered_map<std::string, Model> GetUploadQueue() { return std::move(mUploadQueue); }

    private:
        std::optional<ResourceHandle> LoadModelBinary(const std::string& modelPath);
        std::optional<ResourceHandle> LoadGltf(const std::filesystem::path& modelPath);
        static void LoadGltfNodes(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfMeshes(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfTangents(Model& model);
        static void LoadGltfMaterials(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfTextures(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfImages(Model& model, const fastgltf::Asset& asset);

        std::unordered_map<std::string, Model> mUploadQueue;
        std::vector<ResourceHandle> mHandles;
        fastgltf::Parser mParser;
    };
}  // namespace FS