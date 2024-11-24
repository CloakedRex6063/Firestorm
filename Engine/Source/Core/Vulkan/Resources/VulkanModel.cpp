#include "Core/Render/Vulkan/Resources/VulkanModel.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanModel::VulkanModel(const std::shared_ptr<VulkanContext>& context,
                             const uint64_t verticesSize,
                             const uint64_t indicesSize,
                             std::vector<VulkanMesh>&& meshes,
                             std::vector<Node>&& nodes,
                             std::vector<uint64_t>&& rootNodes,
                             std::vector<Material>&& materials,
                             std::vector<VulkanImage>&& textures)
        : mVertexBuffer(context, BufferType::eVertex, verticesSize),
          mIndexBuffer(context, BufferType::eIndex, indicesSize),
          mMaterialBuffer(context, BufferType::eVertex, materials.size() * sizeof(Material)),
          mRootNodes(std::move(rootNodes)),
          mNodes(std::move(nodes)),
          mMeshes(std::move(meshes)),
          mMaterials(std::move(materials)),
          mTextures(std::move(textures))
    {
        const VkBufferDeviceAddressInfo vertexBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                         .buffer = mVertexBuffer};
        const VkBufferDeviceAddressInfo materialBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                           .buffer = mMaterialBuffer};
        mVertexBufferAddress = vkGetBufferDeviceAddress(*context, &vertexBufferInfo);
        mMaterialBufferAddress = vkGetBufferDeviceAddress(*context, &materialBufferInfo);
    }
} // namespace FS