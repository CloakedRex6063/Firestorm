#pragma once

namespace FS::VK
{
    class Device;
    class Swapchain;
    class Queue
    {
    public:
        Queue(const std::shared_ptr<Device>& device, vk::QueueFlagBits queueType);
        operator vk::raii::Queue&() const { return *mQueue; }

        void SubmitCommand(const vk::SemaphoreSubmitInfo& signalSemaphoreInfo,
                           const vk::SemaphoreSubmitInfo& waitSemaphoreInfo,
                           const vk::CommandBufferSubmitInfo& commandBufferSubmitInfo, vk::Fence fence = {}) const;
        void Present(const Swapchain& swapchain, const vk::raii::Semaphore& semaphore) const;

    private:
        std::shared_ptr<Device> mDevice;
        uint32_t mFamilyIndex;
        std::unique_ptr<vk::raii::Queue> mQueue;
    };
} // namespace FS::VK
