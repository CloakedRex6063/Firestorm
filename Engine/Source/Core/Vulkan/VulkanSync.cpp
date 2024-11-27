#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanFence::VulkanFence(const std::shared_ptr<VulkanContext>& context, const VkFenceCreateFlags flags)
        : mContext(context)
    {
        const VkFenceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags};
        vkCreateFence(*mContext, &info, nullptr, &mFence);
    }

    VulkanFence::~VulkanFence()
    {
        if (mContext)
        {
            vkDestroyFence(*mContext, mFence, nullptr);
        }
    }

    VulkanSemaphore::VulkanSemaphore(const std::shared_ptr<VulkanContext>& context, const VkSemaphoreCreateFlags flags)
        : mContext(context)
    {
        const VkSemaphoreCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .flags = flags};
        vkCreateSemaphore(*mContext, &createInfo, nullptr, &mSemaphore);
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        if (mContext)
            vkDestroySemaphore(*mContext, mSemaphore, nullptr);
    }
} // namespace FS