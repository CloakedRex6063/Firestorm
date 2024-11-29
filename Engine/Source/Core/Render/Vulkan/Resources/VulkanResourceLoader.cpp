#include "Core/Render/Vulkan/Resources/VulkanResourceLoader.h"
#include "Core/ECS.h"
#include "Core/Engine.h"
#include "Core/ResourceManager.h"
#include "Core/Render/Vulkan/VulkanCommand.h"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"

namespace FS
{
    VulkanResourceLoader::VulkanResourceLoader(const std::shared_ptr<VulkanContext>& context)
        : mContext(context),
          mFence(mContext),
          mLightBuffer(context, BufferType::eMappedStorage, sizeof(Component::Light) * 1000),
          mDescriptor(mContext)
    {
        mTransferCommand = std::make_unique<VulkanCommand>(mContext->CreateCommand(mContext->GetTransferQueue()));
        constexpr VkSamplerCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                                    .magFilter = VK_FILTER_LINEAR,
                                                    .minFilter = VK_FILTER_LINEAR};
        vkCreateSampler(*mContext, &createInfo, nullptr, &mLinearSampler);

        mLights.resize(1000);
        mContext->UpdateDescriptorStorageBuffer(mLightBuffer, GetDescriptor(), 0);
        mMappedLightMemory = mContext->MapMemory(mLightBuffer.GetAllocation());
    }

    VulkanResourceLoader::~VulkanResourceLoader()
    {
        mContext->UnmapMemory(mLightBuffer.GetAllocation());
        vkDestroySampler(*mContext, mLinearSampler, nullptr);
    }

    void VulkanResourceLoader::UploadModels()
    {
        auto models = gEngine.ResourceManager().GetUploadQueue();

        if (models.empty()) return;
        
        Log::Info("Uploading {} models", models.size());
        mTransferCommand->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        std::vector<VulkanBuffer> mTexturesStagingBuffers;
        std::vector<VulkanBuffer> mImageStagingBuffers;
        std::vector<VulkanBuffer> mModelStagingBuffers;
        std::vector<VulkanBuffer> mMaterialStagingBuffers;
        mTexturesStagingBuffers.reserve(models.size());
        mImageStagingBuffers.reserve(models.size());
        mModelStagingBuffers.reserve(models.size());
        mMaterialStagingBuffers.reserve(models.size());

        mModels.reserve(mModels.size() + models.size());
        for (auto& model : std::views::values(models))
        {
            std::vector<VulkanImage> images;
            images.reserve(model.mImages.size());
            uint32_t totalImageSize = 0;
            for (const auto& image : model.mImages)
            {
                totalImageSize += image.mWidth * image.mHeight * 4;
            }

            std::unordered_map<uint64_t, uint16_t> incrementorMap;
            mImageStagingBuffers.emplace_back(mContext, BufferType::eStaging, totalImageSize);
            auto& imageBuffer = mImageStagingBuffers.back();
            auto mappedImage = mContext->MapMemory(imageBuffer.GetAllocation());
            uint64_t imageOffset = 0;
            for (const auto [index, pixels] : std::views::enumerate(model.mImages))
            {
                images.emplace_back(
                    mContext,
                    ImageType::e2D,
                    VK_FORMAT_R8G8B8A8_UNORM,
                    VkExtent2D(pixels.mWidth, pixels.mHeight),
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
                auto& vulkanImage = images.back();
                const auto imageSize = pixels.mWidth * pixels.mHeight * 4;

                std::memcpy(static_cast<char*>(mappedImage) + imageOffset, pixels.mPixels.data(), imageSize);
                auto imageBufferCopy =
                    VulkanUtils::GetBufferImageCopy2(imageOffset, VkOffset2D(), VkExtent2D(pixels.mWidth, pixels.mHeight));
                mTransferCommand->TransitionImageLayout(vulkanImage, ImageLayout::eUndefined, ImageLayout::eTransferDst);
                mTransferCommand->CopyBufferToImage(imageBuffer, vulkanImage, imageBufferCopy);
                mTransferCommand->TransitionImageLayout(vulkanImage, ImageLayout::eTransferDst, ImageLayout::eShaderReadOnly);
                mContext->UpdateDescriptorImage(mLinearSampler, vulkanImage.GetView(), GetDescriptor(), mIncrementor);
                incrementorMap[index] = mIncrementor;
                mIncrementor++;
                imageOffset += imageSize;
            }
            mContext->UnmapMemory(imageBuffer.GetAllocation());

            auto totalVerticesSize = static_cast<uint32_t>(model.mVertices.size() * sizeof(Vertex));
            auto totalIndicesSize = static_cast<uint32_t>(model.mIndices.size() * sizeof(uint32_t));
            auto totalModelSize = totalVerticesSize + totalIndicesSize;
            mModelStagingBuffers.emplace_back(mContext, BufferType::eStaging, totalModelSize);
            auto& modelBuffer = mModelStagingBuffers.back();
            const auto mappedModel = mContext->MapMemory(modelBuffer.GetAllocation());
            std::memcpy(mappedModel, model.mVertices.data(), sizeof(Vertex) * model.mVertices.size());
            std::memcpy(static_cast<char*>(mappedModel) + totalVerticesSize, model.mIndices.data(), totalIndicesSize);
            mContext->UnmapMemory(modelBuffer.GetAllocation());

            mModels.emplace_back(mContext,
                                 totalVerticesSize,
                                 totalIndicesSize,
                                 std::move(model.mMeshes),
                                 std::move(model.mNodes),
                                 std::move(model.mRootNodes),
                                 std::move(model.mMaterials),
                                 std::move(model.mTextures),
                                 std::move(model.mSamplers),
                                 std::move(images));
            auto& vulkanModel = mModels.back();

            auto vertexBufferCopy = VulkanUtils::GetBufferCopy2(totalVerticesSize, 0, 0);
            mTransferCommand->CopyBufferToBuffer(modelBuffer, vulkanModel.mVertexBuffer, vertexBufferCopy);
            auto indexBufferCopy = VulkanUtils::GetBufferCopy2(totalIndicesSize, totalVerticesSize, 0);
            mTransferCommand->CopyBufferToBuffer(modelBuffer, vulkanModel.mIndexBuffer, indexBufferCopy);


            auto totalTextureSize = sizeof(Texture) * vulkanModel.mTextures.size();
            mTexturesStagingBuffers.emplace_back(mContext,
                                                 BufferType::eStaging,
                                                 static_cast<uint32_t>(totalTextureSize));
            auto& textureStagingBuffer = mTexturesStagingBuffers.back();
            auto mappedTexture = mContext->MapMemory(textureStagingBuffer.GetAllocation());
            std::memcpy(mappedTexture, vulkanModel.mTextures.data(), totalTextureSize);
            for (auto& [mSamplerIndex, mImageIndex] : model.mTextures)
            {
                mImageIndex = incrementorMap[mImageIndex];
            }
            mContext->UnmapMemory(textureStagingBuffer.GetAllocation());
            auto textureBufferCopy = VulkanUtils::GetBufferCopy2(totalTextureSize,0, 0);
            mTransferCommand->CopyBufferToBuffer(textureStagingBuffer, vulkanModel.mTextureBuffer, textureBufferCopy);

            uint32_t materialSize = static_cast<uint32_t>(vulkanModel.mMaterials.size() * sizeof(Material));
            mMaterialStagingBuffers.emplace_back(mContext, BufferType::eStaging, materialSize);
            auto& materialStagingBuffer = mMaterialStagingBuffers.back();
            const auto mappedMaterial = mContext->MapMemory(materialStagingBuffer.GetAllocation());
            std::memcpy(mappedMaterial, vulkanModel.mMaterials.data(), materialSize);
            mContext->UnmapMemory(materialStagingBuffer.GetAllocation());
            auto materialBufferCopy = VulkanUtils::GetBufferCopy2(materialSize, 0, 0);
            mTransferCommand->CopyBufferToBuffer(materialStagingBuffer, vulkanModel.mMaterialBuffer, materialBufferCopy);
        }

        mTransferCommand->End();

        mContext->GetTransferQueue().SubmitQueue(*mTransferCommand, nullptr, 0, nullptr, 0, mFence);
        mContext->WaitForFence(mFence, std::numeric_limits<uint64_t>::max());
        mContext->ResetFence(mFence);
    }

    void VulkanResourceLoader::UpdateLights()
    {
        size_t count = 0;

        const auto totalSize = gEngine.ECS().View<Component::Light>().size();
        gEngine.ECS().View<Component::Light>().each(
            [&](const Component::Light& light)
            {
                if (count < totalSize)
                {
                    mLights[count++] = light;
                }
            });
        std::memcpy(mMappedLightMemory, mLights.data(), sizeof(Component::Light) * totalSize);
    }
}  // namespace FS