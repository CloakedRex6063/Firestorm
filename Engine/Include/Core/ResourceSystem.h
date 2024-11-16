#pragma once
#include "Render/Resources/Model.hpp"
#include "fastgltf/core.hpp"

namespace FS
{
    struct ResourceHandle
    {
        std::shared_ptr<std::string> mResourceReference;

        bool operator==(const ResourceHandle& other) const
        {
            if (mResourceReference == other.mResourceReference)
            {
                return true;
            }
            return mResourceReference && other.mResourceReference && *mResourceReference == *other.mResourceReference;
        }
    };
} // namespace FS

namespace std
{
    template <> struct hash<FS::ResourceHandle>
    {
        size_t operator()(const FS::ResourceHandle& handle) const noexcept
        {
            // Hash the pointer address of the shared_ptr if it exists
            return handle.mResourceReference ? std::hash<std::string>{}(*handle.mResourceReference) : 0;
        }
    };
} // namespace std

namespace FS
{
    class ResourceSystem
    {
    public:
        ResourceSystem();
        
        std::optional<ResourceHandle> LoadModel(const std::string& filePath);
        [[nodiscard]] std::unordered_map<std::string, Model> GetModelsToUpload();
        [[nodiscard]] std::unordered_set<std::string> GetModelsToRelease();

    private:
        [[nodiscard]] std::optional<Model> LoadGltf(const std::filesystem::path& path) const;
        static void LoadGltfMeshes(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfTextures(Model& model, const fastgltf::Asset& asset);
        static void LoadGltfNodes(Model& model, const fastgltf::Asset& asset);

        std::unique_ptr<fastgltf::Parser> mParser;
        std::unordered_set<ResourceHandle> mModels;
        std::unordered_map<std::string, Model> mModelsToUpload;
    };
} // namespace FS