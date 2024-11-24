#include "Core/Render/Vulkan/VulkanSwapchain.h"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/Resources/VulkanImage.h"

namespace FS
{
    VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<VulkanContext>& device, const glm::uvec2& size)
        : mContext(device), mGraphicsQueue(mContext->GetSharedGraphicsQueue()) 
    {
        CreateSwapchain(size);
    }
    VulkanSwapchain::~VulkanSwapchain() { vkDestroySwapchainKHR(*mContext, mSwapchain, nullptr); }

    void VulkanSwapchain::RecreateSwapchain(const glm::uvec2& size) { CreateSwapchain(size, mSwapchain); }

    void VulkanSwapchain::CreateSwapchain(const glm::uvec2& size, VkSwapchainKHR oldSwapchain)
    {
        mExtent = VkExtent2D(size.x, size.y);
        auto indices = mGraphicsQueue->GetFamilyIndex();
        const VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = mContext->GetSurface(),
            .minImageCount = Constants::MaxFramesInFlight,
            .imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = mExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &indices,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = true,
            .oldSwapchain = oldSwapchain,
        };

        VK_ASSERT(vkCreateSwapchainKHR(*mContext, &createInfo, nullptr, &mSwapchain))
        CreateImages();

        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(*mContext, oldSwapchain, nullptr);
        }
    }

    void VulkanSwapchain::CreateImages()
    {
        std::array<VkImage, Constants::MaxFramesInFlight> images{};
        uint32_t imageCount = images.size();
        vkGetSwapchainImagesKHR(*mContext, mSwapchain, &imageCount, images.data());

        for (uint32_t i = 0; i < imageCount; i++)
        {
            mImages[i] = std::make_unique<VulkanImage>(mContext, images[i], ImageType::e2D, VK_FORMAT_B8G8R8A8_UNORM);
        }
    }

    VkResult VulkanSwapchain::AcquireNextImage(VkSemaphore semaphore)
    {
        return vkAcquireNextImageKHR(*mContext, mSwapchain, 1000000000, semaphore, nullptr, &mCurrentImageIndex);
    }

    VkResult VulkanSwapchain::Present(VkCommandBuffer cmdBuffer, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore,
                                VkFence fence)
    {
        mGraphicsQueue->SubmitQueue(cmdBuffer, waitSemaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                    signalSemaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, fence);
        const VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                              .waitSemaphoreCount = 1,
                                              .pWaitSemaphores = &signalSemaphore,
                                              .swapchainCount = 1,
                                              .pSwapchains = &mSwapchain,
                                              .pImageIndices = &mCurrentImageIndex};

        return vkQueuePresentKHR(*mGraphicsQueue, &presentInfo);
    }
} // namespace FS