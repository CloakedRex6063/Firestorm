#pragma once

namespace FS
{
    class VulkanContext;
    class VulkanFence
    {
    public:
        VulkanFence(const std::shared_ptr<VulkanContext>& context, VkFenceCreateFlags flags = {});
        ~VulkanFence();
        MOVABLE(VulkanFence);
        NON_COPYABLE(VulkanFence);
        UNDERLYING(VkFence, Fence);

    private:
        std::shared_ptr<VulkanContext> mContext;
        VkFence mFence{};
    };

    class VulkanSemaphore
    {
    public:
        VulkanSemaphore(const std::shared_ptr<VulkanContext>& context, VkSemaphoreCreateFlags flags = {});
        ~VulkanSemaphore();
        MOVABLE(VulkanSemaphore);
        NON_COPYABLE(VulkanSemaphore);
        UNDERLYING(VkSemaphore, Semaphore);

    private:
        std::shared_ptr<VulkanContext> mContext;
        VkSemaphore mSemaphore{};
    };
} // namespace FS