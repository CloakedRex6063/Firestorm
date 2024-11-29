#include "Core/Render/Vulkan/Resources/VulkanModel.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanModel::VulkanModel(const std::shared_ptr<VulkanContext>& context,
                             const uint32_t verticesSize,
                             const uint32_t indicesSize,
                             std::vector<Mesh>&& meshes,
                             std::vector<Node>&& nodes,
                             std::vector<uint32_t>&& rootNodes,
                             std::vector<Material>&& materials,
                             std::vector<Texture>&& textures,
                             std::vector<Sampler>&& samplers,
                             std::vector<VulkanImage>&& images)
        : mVertexBuffer(context, BufferType::eVertex, verticesSize),
          mIndexBuffer(context, BufferType::eIndex, indicesSize),
          mMaterialBuffer(context, BufferType::eVertex, materials.size() * sizeof(Material)),
          mTextureBuffer(context, BufferType::eVertex, textures.size() * sizeof(Texture)),
          mRootNodes(std::move(rootNodes)),
          mNodes(std::move(nodes)),
          mMeshes(std::move(meshes)),
          mMaterials(std::move(materials)),
          mTextures(std::move(textures)),
          mSamplers(std::move(samplers)),
          mImages(std::move(images))
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