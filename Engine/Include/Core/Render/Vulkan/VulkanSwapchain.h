#pragma once
#include "Core/Render/Vulkan/Constants.hpp"

namespace FS
{
    class Window;
}

namespace FS
{
    class VulkanImage;
    class VulkanContext;
    class VulkanQueue;
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(const std::shared_ptr<VulkanContext>& device, const glm::uvec2& size);
        ~VulkanSwapchain();

        NON_MOVABLE(VulkanSwapchain);
        NON_COPYABLE(VulkanSwapchain);
        UNDERLYING(VkSwapchainKHR, Swapchain);

        void RecreateSwapchain(const glm::uvec2& size);
        void CreateSwapchain(const glm::uvec2& size, VkSwapchainKHR oldSwapchain = nullptr);
        void CreateImages();

        [[nodiscard]] VkResult AcquireNextImage(VkSemaphore semaphore);
        [[nodiscard]] VkResult Present(VkCommandBuffer cmdBuffer, VkSemaphore waitSemaphore,
                                       VkSemaphore signalSemaphore, VkFence fence);

        [[nodiscard]] uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
        [[nodiscard]] VulkanImage& GetCurrentImage() const { return *mImages[mCurrentImageIndex]; }
        [[nodiscard]] VkExtent2D GetExtent() const { return mExtent; }

    private:
        std::shared_ptr<VulkanContext> mContext;
        std::shared_ptr<VulkanQueue> mGraphicsQueue;

        VkSwapchainKHR mSwapchain{};

        std::array<std::unique_ptr<VulkanImage>, Constants::MaxFramesInFlight> mImages;
        uint32_t mCurrentImageIndex = 0;
        VkExtent2D mExtent{};
    };
} // namespace FS
