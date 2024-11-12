#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    Model::Model(const std::shared_ptr<Device>& device, const std::span<Vertex> vertices,
                 const std::span<uint32_t> indices)
    {
        const auto vertexBufferSize = vertices.size() * sizeof(Vertex);
        const auto indexBufferSize = indices.size() * sizeof(uint32_t);

        const auto vertexBufferFlags = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst |
                                       vk::BufferUsageFlagBits::eShaderDeviceAddress;
        const auto indexBufferFlags = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
        auto vertexBuffer = Buffer(device, vertexBufferSize, vertexBufferFlags, VMA_MEMORY_USAGE_GPU_ONLY);
        auto indexBuffer = Buffer(device, indexBufferSize, indexBufferFlags, VMA_MEMORY_USAGE_GPU_ONLY);
        auto deviceAddress = device->GetBufferAddress(vertexBuffer);
        mModelBuffer = std::make_unique<ModelBuffer>(std::move(vertexBuffer), std::move(indexBuffer), deviceAddress);
    }
} // namespace FS::VK