#pragma once
#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include <Core/Render/Resources/Model.hpp>

namespace FS
{
    struct ModelPushConstant
    {
        glm::mat4 mModel;
        VkDeviceAddress mVertexAddress;
        VkDeviceAddress mMaterialAddress;
        int mMaterialBaseIndex;
    };

    struct VulkanModel
    {
        VulkanModel(const std::shared_ptr<VulkanContext>& context,
                    uint32_t verticesSize,
                    uint32_t indicesSize,
                    uint32_t materialSize,
                    std::vector<VulkanImage>&& images,
                    const std::vector<uint32_t>& mRootNodes,
                    const std::vector<Node>& nodes,
                    const std::vector<Mesh>& meshes);
        NON_COPYABLE(VulkanModel);
        MOVABLE(VulkanModel);

        std::vector<uint32_t> mRootNodes;
        std::vector<Node> mNodes;
        std::vector<Mesh> mMeshes;
        std::vector<VulkanImage> mImages;
        
        VulkanBuffer mVertexBuffer;
        VulkanBuffer mIndexBuffer;
        VulkanBuffer mMaterialBuffer;
        
        VkDeviceAddress mVertexBufferAddress{};
        VkDeviceAddress mMaterialBufferAddress{};
    };
}  // namespace FS