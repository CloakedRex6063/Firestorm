#include "Core/Render/Vulkan/Resources/ModelManager.h"
#include "Core/Render/Vulkan/Command.h"
#include "Core/Render/Vulkan/Context.h"
#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Sync.h"
#include "Core/Render/Vulkan/Resources/Descriptor.h"
#include "Core/Render/Vulkan/Tools/Utils.h"

namespace FS::VK
{
    ModelManager::ModelManager(const std::shared_ptr<Context>& context) : mContext(context), mDescriptor(mContext)
    {
        mTransferCommand = std::make_unique<Command>(mContext->CreateCommand(mContext->GetTransferQueue()));
        VkSamplerCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                          .magFilter = VK_FILTER_LINEAR,
                                          .minFilter = VK_FILTER_LINEAR};
        vkCreateSampler(*mContext, &createInfo, nullptr, &mSampler);
    }

    ModelManager::~ModelManager() { vkDestroySampler(*mContext, mSampler, nullptr); }

    void ModelManager::UploadModels(std::unordered_map<std::string, FS::Model>& models)
    {
        Log::Info("Uploading {} models", models.size());
        mTransferCommand->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        uint64_t totalBufferSize = 0;
        uint64_t totalImageSize = 0;
        for (auto& model : std::views::values(models))
        {
            totalBufferSize += model.mTotalVerticesSize + model.mTotalIndicesSize;
            for (auto& image : model.mTextures)
            {
                totalImageSize += image.mWidth * image.mHeight * 4;
            }
        }
        auto stagingBuffer = Buffer(mContext, BufferType::eStaging, totalBufferSize);
        const auto mapped = mContext->MapMemory(stagingBuffer.GetAllocation());

        auto imageStagingBuffer = Buffer(mContext, BufferType::eStaging, totalImageSize);
        const auto mappedImage = mContext->MapMemory(imageStagingBuffer.GetAllocation());

        mModels.reserve(mModels.size() + models.size());

        uint64_t globalOffset = 0;
        uint64_t globalImageOffset = 0;
        std::unordered_map<uint32_t, uint16_t> textureToIncrementorMap;
        for (auto& model : std::views::values(models))
        {
            uint64_t textureIndexSizeOffset = globalImageOffset;
            std::vector<Image> images;
            images.reserve(model.mTextures.size());
            for (auto [index, texture] : std::views::enumerate(model.mTextures))
            {
                const uint64_t textureSize = texture.mPixels.size() * sizeof(uint8_t);
                std::memcpy(static_cast<char*>(mappedImage) + textureIndexSizeOffset, texture.mPixels.data(), textureSize);
                images.emplace_back(
                    mContext,
                    ImageType::e2D,
                    VK_FORMAT_R8G8B8A8_UNORM,
                    VkExtent2D(texture.mWidth, texture.mHeight),
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
                auto& image = images.back();
                auto copyRegion = Utils::GetBufferImageCopy2(textureIndexSizeOffset,
                                                             VkOffset2D(),
                                                             VkExtent2D(texture.mWidth, texture.mHeight));
                mTransferCommand->TransitionImageLayout(image, ImageLayout::eUndefined, ImageLayout::eTransferDst);
                mTransferCommand->CopyBufferToImage(imageStagingBuffer, image, copyRegion);
                mTransferCommand->TransitionImageLayout(image, ImageLayout::eTransferDst, ImageLayout::eShaderReadOnly);
                mContext->UpdateDescriptorImage(mSampler, image.GetView(), GetDescriptor(), mIncrementor);
                textureToIncrementorMap[index] = mIncrementor;
                ++mIncrementor;
                textureIndexSizeOffset += textureSize;
            }
            globalImageOffset += textureIndexSizeOffset;

            std::vector<Mesh> meshes;
            uint64_t vertexOffset = 0;
            uint64_t indexOffset = 0;
            uint64_t modelVertexSizeOffset = globalOffset;
            for (auto& mesh : model.mMeshes)
            {
                const uint64_t vertexSize = mesh.mVertices.size() * sizeof(Vertex);
                std::memcpy(static_cast<char*>(mapped) + modelVertexSizeOffset, mesh.mVertices.data(), vertexSize);
                modelVertexSizeOffset += vertexSize;

                meshes.emplace_back(vertexOffset, indexOffset, mesh.mIndices.size(), mesh.mMaterialIndex);
                vertexOffset += mesh.mVertices.size();
                indexOffset += mesh.mIndices.size();

                auto& material = model.mMaterials[mesh.mMaterialIndex];
                material.mBaseTextureIndex = textureToIncrementorMap[material.mBaseTextureIndex];
                material.mRoughnessTextureIndex = textureToIncrementorMap[material.mRoughnessTextureIndex];
            }

            uint64_t modelIndexSizeOffset = modelVertexSizeOffset;
            for (auto& mesh : model.mMeshes)
            {
                const uint64_t indexSize = mesh.mIndices.size() * sizeof(uint32_t);
                std::memcpy(static_cast<char*>(mapped) + modelIndexSizeOffset, mesh.mIndices.data(), indexSize);
                modelIndexSizeOffset += indexSize;
            }

            mModels.emplace_back(mContext,
                                 model.mTotalVerticesSize,
                                 model.mTotalIndicesSize,
                                 meshes,
                                 model.mNodes,
                                 model.mRootNodes,
                                 model.mMaterials,
                                 std::move(images));
            auto& newModel = mModels.back();

            VkBufferCopy2 vertexBufferCopy = Utils::GetBufferCopy2(model.mTotalVerticesSize, globalOffset, 0);
            VkBufferCopy2 indexBufferCopy =
                Utils::GetBufferCopy2(model.mTotalIndicesSize, globalOffset + model.mTotalVerticesSize, 0);
            mTransferCommand->CopyBufferToBuffer(stagingBuffer, newModel.GetVertexBuffer(), vertexBufferCopy);
            mTransferCommand->CopyBufferToBuffer(stagingBuffer, newModel.GetIndexBuffer(), indexBufferCopy);
            globalOffset += model.mTotalVerticesSize + model.mTotalIndicesSize;
        }
        mContext->UnmapMemory(imageStagingBuffer.GetAllocation());
        mContext->UnmapMemory(stagingBuffer.GetAllocation());
        mTransferCommand->End();
        const auto fence = mContext->CreateFence();
        mContext->GetTransferQueue().SubmitQueue(*mTransferCommand, nullptr, 0, nullptr, 0, fence);
        mContext->WaitForFence(fence, std::numeric_limits<uint64_t>::max());
    }
}  // namespace FS::VK