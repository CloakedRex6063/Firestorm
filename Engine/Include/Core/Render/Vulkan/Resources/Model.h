#pragma once
#include "Buffer.h"
#include "Image.h"

namespace FS::VK
{
    struct Mesh
    {
        uint32_t mVertexOffset;
        uint32_t mIndexOffset;
        uint32_t mIndexCount;
        int mMaterialIndex;
    };

    struct ModelPushConstant
    {
        glm::mat4 mMVP;
        VkDeviceAddress mVertexAddress;
        int mBaseTextureIndex;
    };

    class Model
    {
    public:
        Model(const std::shared_ptr<Context>& context,
              uint64_t verticesSize,
              uint64_t indicesSize,
              const std::vector<Mesh>& meshes,
              const std::vector<Node>& nodes,
              const std::vector<int>& rootNodes,
              const std::vector<Material>& materials,
              std::vector<Image> textures);
        NON_COPYABLE(Model);
        MOVABLE(Model);

        [[nodiscard]] Buffer& GetVertexBuffer() { return mVertexBuffer; }
        [[nodiscard]] Buffer& GetIndexBuffer() { return mIndexBuffer; }
        [[nodiscard]] VkDeviceAddress GetVertexBufferAddress() const { return mVertexBufferAddress; }

        [[nodiscard]] std::span<int> GetRootNodeIndices() { return mRootNodes; }
        [[nodiscard]] std::span<Node> GetNodes() { return mNodes; }
        [[nodiscard]] std::span<Material> GetMaterials() { return mMaterials; }
        [[nodiscard]] std::span<Image> GetTextures() { return mTextures; }
        [[nodiscard]] std::span<Mesh> GetMeshes() { return mMeshes; }

    private:
        Buffer mVertexBuffer;
        Buffer mIndexBuffer;
        VkDeviceAddress mVertexBufferAddress{};
        std::vector<int> mRootNodes;
        std::vector<Node> mNodes;
        std::vector<Mesh> mMeshes;
        std::vector<Material> mMaterials;
        std::vector<Image> mTextures;
    };
}  // namespace FS::VK