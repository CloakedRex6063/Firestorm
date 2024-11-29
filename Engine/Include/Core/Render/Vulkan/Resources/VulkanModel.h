#pragma once
#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace FS
{
    struct ModelPushConstant
    {
        glm::mat4 mModel;
        VkDeviceAddress mVertexAddress;
        VkDeviceAddress mMaterialAddress;
        VkDeviceAddress mTextureAddress;
        int mMaterialBaseIndex;
    };

    class VulkanModel
    {
    public:
        VulkanModel(const std::shared_ptr<VulkanContext>& context,
                    uint32_t verticesSize,
                    uint32_t indicesSize,
                    std::vector<Mesh>&& meshes,
                    std::vector<Node>&& nodes,
                    std::vector<uint32_t>&& rootNodes,
                    std::vector<Material>&& materials,
                    std::vector<Texture>&& textures,
                    std::vector<Sampler>&& samplers,
                    std::vector<VulkanImage>&& images);
        NON_COPYABLE(VulkanModel);
        MOVABLE(VulkanModel);

        VulkanBuffer mVertexBuffer;
        VulkanBuffer mIndexBuffer;
        VulkanBuffer mMaterialBuffer;
        VulkanBuffer mTextureBuffer;
        VkDeviceAddress mVertexBufferAddress{};
        VkDeviceAddress mMaterialBufferAddress{};
        VkDeviceAddress mTextureBufferAddress{};

        std::vector<uint32_t> mRootNodes;
        std::vector<Node> mNodes;
        std::vector<Mesh> mMeshes;
        std::vector<Material> mMaterials;
        std::vector<Texture> mTextures;
        std::vector<Sampler> mSamplers;
        std::vector<VulkanImage> mImages;
    };
}  // namespace FS