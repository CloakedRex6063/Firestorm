#pragma once
#include "VulkanDescriptor.h"
#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/Resources/VulkanModel.h"

namespace FS
{
    class VulkanContext;
    class VulkanCommand;
    class VulkanResourceLoader
    {
    public:
        VulkanResourceLoader(const std::shared_ptr<VulkanContext>& context);
        ~VulkanResourceLoader();

        void UploadModels();
        [[nodiscard]] std::span<VulkanModel> GetModels() { return mModels; }
        [[nodiscard]] VulkanDescriptor& GetDescriptor() { return mDescriptor; }

        void UpdateLights();

    private:
        std::shared_ptr<VulkanContext> mContext;
        std::unique_ptr<VulkanCommand> mTransferCommand;
        VulkanFence mFence;
        std::vector<VulkanModel> mModels;

        std::vector<Component::Light> mLights;

        VulkanBuffer mLightBuffer;
        void* mMappedLightMemory;

        VulkanDescriptor mDescriptor;
        VkSampler mLinearSampler{};

        uint16_t mIncrementor = 0;
    };
}  // namespace FS