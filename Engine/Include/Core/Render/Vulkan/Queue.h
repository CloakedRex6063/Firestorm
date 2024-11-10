#pragma once

namespace FS::VK 
{
    class Device;
    class Swapchain;
    class Queue
    {
    public:
        Queue() = delete;
        Queue(const std::shared_ptr<Device>& device, vk::QueueFlagBits queueType);
        operator vk::raii::Queue&() const
        {
            return *mQueue;
        }

        void SubmitCommand(vk::SemaphoreSubmitInfo& signalSemaphoreInfo,
                           vk::SemaphoreSubmitInfo& waitSemaphoreInfo,
                           vk::CommandBufferSubmitInfo& commandBufferSubmitInfo) const;
        void Present(const Swapchain& swapchain, const vk::raii::Semaphore& semaphore) const;

    private:
        std::shared_ptr<Device> mDevice;
        uint32_t mFamilyIndex;
        std::unique_ptr<vk::raii::Queue> mQueue;
    };
}
