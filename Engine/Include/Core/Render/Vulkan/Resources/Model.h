#pragma once
#include "Core/Render/Resources/Model.hpp"
#include "Core/Render/Vulkan/Resources/Buffer.h"

namespace FS::VK
{
    struct ModelBuffer
    {
        Buffer mVertexBuffer;
        Buffer mIndexBuffer;
        vk::DeviceAddress mVertexBufferAddress{};
        ModelBuffer(Buffer&& vertexBuffer, Buffer&& indexBuffer, const vk::DeviceAddress deviceAddress)
            : mVertexBuffer(std::move(vertexBuffer)), mIndexBuffer(std::move(indexBuffer)),
              mVertexBufferAddress(deviceAddress)
        {
        }
    };

    struct MeshPushConstants
    {
        glm::mat4 mWorldMatrix;
        vk::DeviceAddress mVertexBufferAddress;
    };

    class Device;
    class Model
    {
    public:
        Model(const std::shared_ptr<Device>& device, std::span<Vertex> vertices, std::span<uint32_t> indices);
        MOVABLE(Model);
        NON_COPYABLE(Model);
        
        [[nodiscard]] ModelBuffer& GetModelBuffer() const { return *mModelBuffer; }

    private:
        std::unique_ptr<ModelBuffer> mModelBuffer;
    };
} // namespace FS::VK