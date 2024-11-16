#pragma once

namespace FS::VK
{
    class Queue
    {
    public:
        Queue(const VkQueue& queue, const uint32_t familyIndex)
            : mQueue(queue), mFamilyIndex(familyIndex)
        {
        }
        NON_MOVABLE(Queue);
        NON_COPYABLE(Queue);
        UNDERLYING(VkQueue, Queue)

        [[nodiscard]] uint32_t GetFamilyIndex() const { return mFamilyIndex; }

        void SubmitQueue(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags2 waitStageMask,
                         VkSemaphore signalSemaphore, VkPipelineStageFlags2 signalStageMask, VkFence fence) const;

    private:
        VkQueue mQueue;
        uint32_t mFamilyIndex;
    };
} // namespace FS::VK
