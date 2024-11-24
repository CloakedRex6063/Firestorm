#pragma once
#include "VulkanDescriptor.h"
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
        
        void UploadModels(std::unordered_map<std::string, Model>& models);
        [[nodiscard]] std::span<VulkanModel> GetModels() { return mModels; }
        [[nodiscard]] VulkanDescriptor& GetDescriptor() { return mDescriptor; }
        
    private:
        std::shared_ptr<VulkanContext> mContext;
        std::unique_ptr<VulkanCommand> mTransferCommand;
        std::vector<VulkanModel> mModels;

        VulkanDescriptor mDescriptor;
        VkSampler mLinearSampler{};

        uint16_t mIncrementor = 0;
    };
}