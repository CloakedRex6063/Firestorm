#include "Core/Render/Vulkan/Sync.h"
#include "Core/Render/Vulkan/Context.h"

namespace FS::VK
{
    Fence::Fence(const std::shared_ptr<Context>& context, const VkFenceCreateFlags flags)
        : mContext(context)
    {
        const VkFenceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags};
        vkCreateFence(*mContext, &info, nullptr, &mFence);
    }

    Fence::~Fence()
    {
        if (mContext)
        {
            vkDestroyFence(*mContext, mFence, nullptr);
        }
    }

    Semaphore::Semaphore(const std::shared_ptr<Context>& context, const VkSemaphoreCreateFlags flags)
        : mContext(context)
    {
        const VkSemaphoreCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .flags = flags};
        vkCreateSemaphore(*mContext, &createInfo, nullptr, &mSemaphore);
    }

    Semaphore::~Semaphore()
    {
        if (mContext)
            vkDestroySemaphore(*mContext, mSemaphore, nullptr);
    }
} // namespace FS::VK