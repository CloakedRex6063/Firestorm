#include "Core/Render/Vulkan/Resources/ModelManager.h"
#include "Core/Engine.h"
#include "Core/Render/Vulkan/Command.h"
#include "Core/Render/Vulkan/Device.h"
#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/ResourceSystem.h"

namespace FS::VK
{
    ModelManager::ModelManager(const std::shared_ptr<Device>& device, const std::shared_ptr<Queue>& queue)
        : mDevice(device), mTransferQueue(queue)
    {
        mCommand = std::make_unique<Command>(device);
    }

    void ModelManager::UploadMeshes()
    {
        const auto& models = gEngine.ResourceSystem().GetModelsToUpload();
        for (const auto& [path, model] : models)
        {
            uint32_t allocSize = 0;
            uint32_t totalVertexSize = 0;
            uint32_t totalIndexSize = 0;

            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            // Calculate total allocation size for the entire model (vertex + index data)
            for (const auto& mesh : model->mMeshes)
            {
                const auto vertexSize = mesh.mVertices.size() * sizeof(Vertex);
                const auto indexSize = mesh.mIndices.size() * sizeof(uint32_t);
                totalVertexSize += vertexSize;
                totalIndexSize += indexSize;
                allocSize += vertexSize + indexSize;
                vertices.insert(vertices.end(), mesh.mVertices.begin(), mesh.mVertices.end());
                indices.insert(indices.end(), mesh.mIndices.begin(), mesh.mIndices.end());
            }

            mModels.emplace(path, Model(mDevice, vertices, indices));

            constexpr auto stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
            const auto stagingBuffer = Buffer(mDevice, allocSize, stagingUsage, VMA_MEMORY_USAGE_CPU_ONLY);

            const auto memory = mDevice->MapMemory(stagingBuffer.GetAllocation());
            std::memcpy(memory, vertices.data(), vertices.size() * sizeof(Vertex));
            std::memcpy(static_cast<char*>(memory) + vertices.size(), indices.data(),
                        indices.size() * sizeof(uint32_t));
            mDevice->UnmapMemory(stagingBuffer.GetAllocation());

            const auto vertexCopyRegion = vk::BufferCopy2().setSize(totalVertexSize);
            const auto indexCopyRegion = vk::BufferCopy2().setDstOffset(totalVertexSize).setSize(totalIndexSize);

            auto& vulkanModel = mModels.find(path)->second;

            mCommand->CopyBuffer(stagingBuffer, vulkanModel.GetModelBuffer().mVertexBuffer, vertexCopyRegion);
            mCommand->CopyBuffer(stagingBuffer, vulkanModel.GetModelBuffer().mIndexBuffer, indexCopyRegion);

            auto fence = mDevice->CreateFence();
            const auto commandSubmitInfo = mCommand->GetSubmitInfo();
            mTransferQueue->SubmitCommand(vk::SemaphoreSubmitInfo(), vk::SemaphoreSubmitInfo(), commandSubmitInfo,
                                          fence);
            mDevice->WaitForFence(fence, std::numeric_limits<uint64_t>::max());
        }
    }
} // namespace FS::VK