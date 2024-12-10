#pragma once

namespace FS
{
    class VulkanQueue
    {
    public:
        VulkanQueue(const VkQueue& queue, const uint32_t familyIndex)
            : mQueue(queue), mFamilyIndex(familyIndex)
        {
        }
        NON_MOVABLE(VulkanQueue);
        NON_COPYABLE(VulkanQueue);
        UNDERLYING(VkQueue, Queue)

        [[nodiscard]] uint32_t GetFamilyIndex() const { return mFamilyIndex; }

        void SubmitQueue(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags2 waitStageMask,
                         VkSemaphore signalSemaphore, VkPipelineStageFlags2 signalStageMask, VkFence fence) const;
        void SubmitQueueForHost(VkCommandBuffer commandBuffer, VkFence fence) const;

    private:
        VkQueue mQueue;
        uint32_t mFamilyIndex;
    };
} // namespace FS
