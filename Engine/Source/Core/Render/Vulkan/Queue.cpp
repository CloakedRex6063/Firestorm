#include "Core/Render/Vulkan/Queue.h"

#include <iostream>
#include <ostream>

#include "Core/Render/Vulkan/Device.h"
#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Utils.h"

namespace FS::VK 
{
    Queue::Queue(const std::shared_ptr<Device>& device, const vk::QueueFlagBits queueType) : mDevice(device)
    {
        mFamilyIndex = mDevice->GetQueueFamily(queueType);
        mQueue = std::make_unique<vk::raii::Queue>(*mDevice, mFamilyIndex, 0);
    }

    void Queue::SubmitCommand(vk::SemaphoreSubmitInfo& signalSemaphoreInfo, vk::SemaphoreSubmitInfo& waitSemaphoreInfo,
                              vk::CommandBufferSubmitInfo& commandBufferSubmitInfo) const
    {
        vk::SubmitInfo2 submitInfo;
        submitInfo.setSignalSemaphoreInfoCount(signalSemaphoreInfo.semaphore ? 1 : 0);
        submitInfo.setSignalSemaphoreInfos(signalSemaphoreInfo);
        submitInfo.setWaitSemaphoreInfoCount(waitSemaphoreInfo.semaphore ? 1 : 0);
        submitInfo.setWaitSemaphoreInfos(waitSemaphoreInfo);
        submitInfo.setCommandBufferInfoCount(commandBufferSubmitInfo.commandBuffer ? 1 : 0);
        submitInfo.setCommandBufferInfos(commandBufferSubmitInfo);
        mQueue->submit2(submitInfo);
    }

    void Queue::Present(const Swapchain& swapchain, const vk::raii::Semaphore& semaphore) const
    {
        vk::PresentInfoKHR presentInfo;
        presentInfo.setSwapchainCount(1);
        const vk::raii::SwapchainKHR& currentSwapchain = swapchain;
        presentInfo.setSwapchains(*currentSwapchain);
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(&*semaphore);
        const auto imageIndex = swapchain.GetCurrentImageIndex();
        presentInfo.setPImageIndices(&imageIndex);
        auto result = mQueue->presentKHR(presentInfo);
    }
} // FS::Vulkan