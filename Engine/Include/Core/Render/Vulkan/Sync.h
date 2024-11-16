#pragma once

namespace FS::VK
{
    class Context;
    class Fence
    {
    public:
        Fence(const std::shared_ptr<Context>& context, VkFenceCreateFlags flags = {});
        ~Fence();
        MOVABLE(Fence);
        NON_COPYABLE(Fence);
        UNDERLYING(VkFence, Fence);

    private:
        std::shared_ptr<Context> mContext;
        VkFence mFence{};
    };

    class Semaphore
    {
    public:
        Semaphore(const std::shared_ptr<Context>& context, VkSemaphoreCreateFlags flags = {});
        ~Semaphore();
        MOVABLE(Semaphore);
        NON_COPYABLE(Semaphore);
        UNDERLYING(VkSemaphore, Semaphore);

    private:
        std::shared_ptr<Context> mContext;
        VkSemaphore mSemaphore{};
    };
} // namespace FS::VK