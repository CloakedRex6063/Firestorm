#include "Core/Render/Vulkan/Resources/VulkanModel.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanModel::VulkanModel(const std::shared_ptr<VulkanContext>& context,
                             const uint32_t verticesSize,
                             const uint32_t indicesSize,
                             const uint32_t materialSize,
                             const uint32_t textureSize,
                             std::vector<VulkanImage>&& images,
                             const std::vector<uint32_t>& mRootNodes,
                             const std::vector<Node>& nodes,
                             const std::vector<Mesh>& meshes)
        : mRootNodes(mRootNodes),
          mNodes(nodes),
          mMeshes(meshes),
          mImages(std::move(images)),
          mVertexBuffer(context, BufferType::eVertex, verticesSize),
          mIndexBuffer(context, BufferType::eIndex, indicesSize),
          mMaterialBuffer(context, BufferType::eMappedStorage, materialSize),
          mTextureBuffer(context, BufferType::eMappedStorage, textureSize)
    {
        const VkBufferDeviceAddressInfo vertexBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                         .buffer = mVertexBuffer};
        const VkBufferDeviceAddressInfo materialBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                           .buffer = mMaterialBuffer};
        const VkBufferDeviceAddressInfo textureBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                          .buffer = mTextureBuffer};
        mVertexBufferAddress = vkGetBufferDeviceAddress(*context, &vertexBufferInfo);
        mMaterialBufferAddress = vkGetBufferDeviceAddress(*context, &materialBufferInfo);
        mTextureBufferAddress = vkGetBufferDeviceAddress(*context, &textureBufferInfo);
    }
}  // namespace FS