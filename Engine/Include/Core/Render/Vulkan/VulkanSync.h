#pragma once

namespace FS
{
    class VulkanContext;
    class Fence
    {
    public:
        Fence(const std::shared_ptr<VulkanContext>& context, VkFenceCreateFlags flags = {});
        ~Fence();
        MOVABLE(Fence);
        NON_COPYABLE(Fence);
        UNDERLYING(VkFence, Fence);

    private:
        std::shared_ptr<VulkanContext> mContext;
        VkFence mFence{};
    };

    class Semaphore
    {
    public:
        Semaphore(const std::shared_ptr<VulkanContext>& context, VkSemaphoreCreateFlags flags = {});
        ~Semaphore();
        MOVABLE(Semaphore);
        NON_COPYABLE(Semaphore);
        UNDERLYING(VkSemaphore, Semaphore);

    private:
        std::shared_ptr<VulkanContext> mContext;
        VkSemaphore mSemaphore{};
    };
} // namespace FS