#include "Core/Render/Vulkan/Resources/VulkanResourceLoader.h"
#include "Core/Render/Vulkan/VulkanCommand.h"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"

namespace FS
{
    VulkanResourceLoader::VulkanResourceLoader(const std::shared_ptr<VulkanContext>& context) : mContext(context),
        mDescriptor(mContext)
    {
        mTransferCommand = std::make_unique<VulkanCommand>(mContext->CreateCommand(mContext->GetTransferQueue()));
        VkSamplerCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                          .magFilter = VK_FILTER_LINEAR,
                                          .minFilter = VK_FILTER_LINEAR};
        vkCreateSampler(*mContext, &createInfo, nullptr, &mLinearSampler);
    }

    VulkanResourceLoader::~VulkanResourceLoader() { vkDestroySampler(*mContext, mLinearSampler, nullptr); }

    void VulkanResourceLoader::UploadModels(std::unordered_map<std::string, Model>& models)
    {
        Log::Info("Uploading {} models", models.size());
        mTransferCommand->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        std::vector<VulkanBuffer> mImageStagingBuffers;
        std::vector<VulkanBuffer> mModelStagingBuffers;
        std::vector<VulkanBuffer> mMaterialStagingBuffers;
        mImageStagingBuffers.reserve(models.size());
        mModelStagingBuffers.reserve(models.size());
        mMaterialStagingBuffers.reserve(models.size());

        mModels.reserve(mModels.size() + models.size());
        for (auto& model : std::views::values(models))
        {
            std::vector<VulkanImage> images;
            images.reserve(model.mTextures.size());
            uint32_t totalTextureSize = 0;
            for (const auto& texture : model.mTextures)
            {
                totalTextureSize += texture.mWidth * texture.mHeight * 4;
            }

            std::unordered_map<uint64_t, uint16_t> incrementorMap;
            mImageStagingBuffers.emplace_back(mContext, BufferType::eStaging, totalTextureSize);
            auto& imageBuffer = mImageStagingBuffers.back();
            auto mappedImage = mContext->MapMemory(imageBuffer.GetAllocation());
            uint64_t textureOffset = 0;
            for (const auto [index, texture] : std::views::enumerate(model.mTextures))
            {
                images.emplace_back(mContext,
                                    ImageType::e2D,
                                    VK_FORMAT_R8G8B8A8_UNORM,
                                    VkExtent2D(texture.mWidth, texture.mHeight),
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT);
                auto& vulkanImage = images.back();
                const auto textureSize = texture.mWidth * texture.mHeight * 4;
                std::memcpy(static_cast<char*>(mappedImage) + textureOffset, texture.mPixels.data(), textureSize);
                auto imageBufferCopy = VulkanUtils::GetBufferImageCopy2(textureOffset, VkOffset2D(), VkExtent2D(texture.mWidth, texture.mHeight));
                mTransferCommand->TransitionImageLayout(vulkanImage, ImageLayout::eUndefined, ImageLayout::eTransferDst);
                mTransferCommand->CopyBufferToImage(imageBuffer, vulkanImage, imageBufferCopy);
                mTransferCommand->TransitionImageLayout(vulkanImage, ImageLayout::eTransferDst, ImageLayout::eShaderReadOnly);
                mContext->UpdateDescriptorImage(mLinearSampler, vulkanImage.GetView(), GetDescriptor(), mIncrementor);
                incrementorMap[index] = mIncrementor;
                mIncrementor++;
                textureOffset += textureSize;
            }
            mContext->UnmapMemory(imageBuffer.GetAllocation());

            for (auto& material : model.mMaterials)
            {
                material.mBaseTextureIndex = incrementorMap[material.mBaseTextureIndex];
                material.mRoughnessTextureIndex = incrementorMap[material.mRoughnessTextureIndex];
            }

            uint32_t vertexOffset = 0;
            uint32_t indexOffset = 0;
            mModelStagingBuffers.emplace_back(mContext,
                                              BufferType::eStaging,
                                              model.mTotalVerticesSize + model.mTotalIndicesSize);
            auto& modelBuffer = mModelStagingBuffers.back();
            std::vector<VulkanMesh> meshes;
            meshes.reserve(model.mMeshes.size());
            const auto mappedModel = mContext->MapMemory(modelBuffer.GetAllocation());
            for (auto& mesh : model.mMeshes)
            {
                meshes.emplace_back(vertexOffset,
                                    indexOffset,
                                    static_cast<uint32_t>(mesh.mIndices.size()),
                                    mesh.mMaterialIndex);
                std::memcpy(static_cast<char*>(mappedModel) + vertexOffset * sizeof(Vertex),
                            mesh.mVertices.data(),
                            mesh.mVertices.size() * sizeof(Vertex));

                vertexOffset += mesh.mVertices.size();
                indexOffset += mesh.mIndices.size();
            }
            mModels.emplace_back(mContext,
                                 model.mTotalVerticesSize,
                                 model.mTotalIndicesSize,
                                 std::move(meshes),
                                 std::move(model.mNodes),
                                 std::move(model.mRootNodes),
                                 std::move(model.mMaterials),
                                 std::move(images));
            auto& vulkanModel = mModels.back();
            auto vertexBufferCopy = VulkanUtils::GetBufferCopy2(model.mTotalVerticesSize, 0, 0);
            mTransferCommand->CopyBufferToBuffer(modelBuffer, vulkanModel.GetVertexBuffer(), vertexBufferCopy);

            indexOffset = 0;
            for (auto& mesh : model.mMeshes)
            {
                std::memcpy(static_cast<char*>(mappedModel) + model.mTotalVerticesSize + indexOffset,
                            mesh.mIndices.data(),
                            mesh.mIndices.size() * sizeof(uint32_t));
                indexOffset += mesh.mIndices.size() * sizeof(uint32_t);
            }
            auto indexBufferCopy = VulkanUtils::GetBufferCopy2(model.mTotalIndicesSize, model.mTotalVerticesSize, 0);
            mTransferCommand->CopyBufferToBuffer(modelBuffer, vulkanModel.GetIndexBuffer(), indexBufferCopy);

            mContext->UnmapMemory(modelBuffer.GetAllocation());

            uint32_t materialSize = vulkanModel.GetMaterials().size() * sizeof(Material);
            mMaterialStagingBuffers.emplace_back(mContext, BufferType::eStaging, materialSize);
            auto& materialBuffer = mMaterialStagingBuffers.back();
            const auto mappedMaterial = mContext->MapMemory(materialBuffer.GetAllocation());
            std::memcpy(mappedMaterial, vulkanModel.GetMaterials().data(), materialSize);
            mContext->UnmapMemory(materialBuffer.GetAllocation());
            auto materialBufferCopy = VulkanUtils::GetBufferCopy2(materialSize, 0, 0);
            mTransferCommand->CopyBufferToBuffer(materialBuffer, vulkanModel.GetMaterialBuffer(), materialBufferCopy);
        }

        mTransferCommand->End();
        const auto fence = mContext->CreateFence();
        mContext->GetTransferQueue().SubmitQueue(*mTransferCommand, nullptr, 0, nullptr, 0, fence);
        mContext->WaitForFence(fence, std::numeric_limits<uint64_t>::max());
    }
} // namespace FS::VK