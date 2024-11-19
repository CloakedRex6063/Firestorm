#pragma once

namespace FS
{
    struct Vertex
    {
        glm::vec3 mPosition{};
        float mUVx{};
        glm::vec3 mNormal{};
        float mUVy{};
        glm::vec4 mColor{};
    };

    struct Node
    {
        glm::mat4 mTransform{};
        int mMeshIndex{};
        std::vector<size_t> mChildren{};
    };

    struct Material
    {
        glm::vec4 mBaseColorFactor{};
        float mMetallicFactor{};
        float mRoughnessFactor{};
        int mBaseTextureIndex{};
        int mRoughnessTextureIndex{};
    };

    struct Mesh
    {
        std::vector<Vertex> mVertices{};
        std::vector<uint32_t> mIndices{};
        int mMaterialIndex{};
    };

    struct Texture
    {
        int mWidth {};
        int mHeight {};
        std::vector<uint8_t> mPixels{};
    };
    
    struct Model
    {
        std::vector<int> mRootNodes;
        std::vector<Node> mNodes{};
        std::vector<Mesh> mMeshes{};
        std::vector<Material> mMaterials{};
        std::vector<Texture> mTextures{};

        uint64_t mTotalIndicesSize{};
        uint64_t mTotalVerticesSize{};
    };
    

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
}  // namespace FS

template <>
struct std::hash<FS::ResourceHandle>
{
    size_t operator()(const FS::ResourceHandle& handle) const noexcept
    {
        return handle.mResourceReference ? std::hash<std::string>{}(*handle.mResourceReference) : 0;
    }
};  // namespace std