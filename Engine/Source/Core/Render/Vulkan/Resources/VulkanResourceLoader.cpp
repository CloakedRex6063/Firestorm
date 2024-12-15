#include "Core/Render/Vulkan/Resources/VulkanResourceLoader.h"
#include "Core/ECS.h"
#include "Core/Engine.h"
#include "Core/ResourceManager.h"
#include "Core/Render/Vulkan/VulkanCommand.h"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"

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

        ktxVulkanDeviceInfo_Construct(&mDeviceInfo,
                                      mContext->GetPhysicalDevice(),
                                      mContext->GetDevice(),
                                      mContext->GetTransferQueue(),
                                      mTransferCommand->GetCommandPool(),
                                      nullptr);
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

        mModels.reserve(mModels.size() + models.size());
        for (auto& [path, model] : models)
        {
            std::vector<VulkanImage> images;
            images.reserve(model.mImageURIs.size());
            for (const auto& [index, image] : std::views::enumerate(model.mImageURIs))
            {
                auto filePath = std::filesystem::path(path).parent_path().string();
                auto texturePath = filePath.append("/").append(image);
                images.emplace_back(mContext, texturePath, mDeviceInfo);
                auto& vulkanImage = images.back();

                mContext->UpdateDescriptorImage(mLinearSampler, vulkanImage.GetView(), GetDescriptor(), mIncrementor);
                mIncrementorMap[index] = mIncrementor++;
            }

            for (auto& material : model.mMaterials)
            {
                material.mBaseTextureIndex = GetIncrementorIndex(material.mBaseTextureIndex);
                material.mEmissiveTextureIndex = GetIncrementorIndex(material.mEmissiveTextureIndex);
                material.mNormalTextureIndex = GetIncrementorIndex(material.mNormalTextureIndex);
                material.mOcclusionTextureIndex = GetIncrementorIndex(material.mOcclusionTextureIndex);
                material.mRoughnessTextureIndex = GetIncrementorIndex(material.mRoughnessTextureIndex);
            }

            auto verticesSize = static_cast<uint32_t>(model.mVertices.size() * sizeof(Vertex));
            auto indicesSize = static_cast<uint32_t>(model.mIndices.size() * sizeof(uint32_t));
            auto materialSize = static_cast<uint32_t>(model.mMaterials.size() * sizeof(Material));

            mModels.emplace_back(mContext,
                                 verticesSize,
                                 indicesSize,
                                 materialSize,
                                 std::move(images),
                                 model.mRootNodes,
                                 model.mNodes,
                                 model.mMeshes);
            auto& vulkanModel = mModels.back();

            mContext->CopyToDeviceBuffer(vulkanModel.mVertexBuffer.GetAllocation(), model.mVertices.data(), 0, verticesSize);
            mContext->CopyToDeviceBuffer(vulkanModel.mIndexBuffer.GetAllocation(), model.mIndices.data(), 0, indicesSize);
            mContext->CopyToDeviceBuffer(vulkanModel.mMaterialBuffer.GetAllocation(), model.mMaterials.data(), 0, materialSize);
        }

        mTransferCommand->End();

        mContext->GetTransferQueue().SubmitQueueForHost(*mTransferCommand, mFence);
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
    
    inline int VulkanResourceLoader::GetIncrementorIndex(const uint32_t index) const
    {
        if (!mIncrementorMap.contains(index)) return -1;
        return mIncrementorMap.at(index);
    }
}  // namespace FS