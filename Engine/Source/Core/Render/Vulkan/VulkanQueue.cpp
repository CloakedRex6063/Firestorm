#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"

namespace FS
{
    void VulkanQueue::SubmitQueue(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore,
                            const VkPipelineStageFlags2 waitStageMask, VkSemaphore signalSemaphore,
                            const VkPipelineStageFlags2 signalStageMask, VkFence fence) const
    {
        const auto commandSubmitInfo = VulkanUtils::GetCommandBufferSubmitInfo(commandBuffer);
        const auto waitSubmitInfo = VulkanUtils::GetSemaphoreSubmitInfo(waitSemaphore, waitStageMask);
        const auto signalSubmitInfo = VulkanUtils::GetSemaphoreSubmitInfo(signalSemaphore, signalStageMask);
        const VkSubmitInfo2 submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = waitSemaphore ? 1u : 0u,
            .pWaitSemaphoreInfos = &waitSubmitInfo,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandSubmitInfo,
            .signalSemaphoreInfoCount = signalSemaphore ? 1u : 0u,
            .pSignalSemaphoreInfos = &signalSubmitInfo,
        };
        vkQueueSubmit2(mQueue, 1, &submitInfo, fence);
    }
} // namespace FS