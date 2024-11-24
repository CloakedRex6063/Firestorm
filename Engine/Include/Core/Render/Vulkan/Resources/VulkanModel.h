#pragma once
#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace FS
{
    struct VulkanMesh
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
        VkDeviceAddress mMaterialAddress;
        int mMaterialBaseIndex;
    };

    class VulkanModel
    {
    public:
        VulkanModel(const std::shared_ptr<VulkanContext>& context,
                    uint64_t verticesSize,
                    uint64_t indicesSize,
                    std::vector<VulkanMesh>&& meshes,
                    std::vector<Node>&& nodes,
                    std::vector<uint64_t>&& rootNodes,
                    std::vector<Material>&& materials,
                    std::vector<VulkanImage>&& textures);
        NON_COPYABLE(VulkanModel);
        MOVABLE(VulkanModel);

        [[nodiscard]] VulkanBuffer& GetVertexBuffer() { return mVertexBuffer; }
        [[nodiscard]] VulkanBuffer& GetIndexBuffer() { return mIndexBuffer; }
        [[nodiscard]] VulkanBuffer& GetMaterialBuffer() { return mMaterialBuffer; }
        [[nodiscard]] VkDeviceAddress GetVertexBufferAddress() const { return mVertexBufferAddress; }
        [[nodiscard]] VkDeviceAddress GetMaterialBufferAddress() const { return mMaterialBufferAddress; }

        [[nodiscard]] std::span<uint64_t> GetRootNodeIndices() { return mRootNodes; }
        [[nodiscard]] std::span<Node> GetNodes() { return mNodes; }
        [[nodiscard]] std::span<Material> GetMaterials() { return mMaterials; }
        [[nodiscard]] std::span<VulkanImage> GetTextures() { return mTextures; }
        [[nodiscard]] std::span<VulkanMesh> GetMeshes() { return mMeshes; }

    private:
        VulkanBuffer mVertexBuffer;
        VulkanBuffer mIndexBuffer;
        VulkanBuffer mMaterialBuffer;
        VkDeviceAddress mVertexBufferAddress{};
        VkDeviceAddress mMaterialBufferAddress{};
        
        std::vector<uint64_t> mRootNodes;
        std::vector<Node> mNodes;
        std::vector<VulkanMesh> mMeshes;
        std::vector<Material> mMaterials;
        std::vector<VulkanImage> mTextures;
    };
} // namespace FS