#pragma once 

namespace FS
{
    class VulkanContext;
    class VulkanDescriptor 
    {
    public:
        VulkanDescriptor(const std::shared_ptr<VulkanContext>& context);
        ~VulkanDescriptor();
        NON_COPYABLE(VulkanDescriptor)
        NON_MOVABLE(VulkanDescriptor)
        UNDERLYING(VkDescriptorSet, Set);

        [[nodiscard]] VkDescriptorSet GetSet() const { return mSet; }
        [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return mLayout; }
        [[nodiscard]] VkDescriptorPool GetPool() const { return mPool; }

    private:
        std::shared_ptr<VulkanContext> mContext;

        VkDescriptorPool mPool;
        VkDescriptorSetLayout mLayout;
        VkDescriptorSet mSet;
    };
}