#pragma once
#include <Core/EnginePCH.hpp>

namespace FS
{
    class Window;
}

namespace FS::VK
{
    class Image;
    class Context;
    class Queue;
    class Swapchain
    {
    public:
        Swapchain(const std::shared_ptr<Context>& device, const glm::uvec2& size);
        ~Swapchain();

        NON_MOVABLE(Swapchain);
        NON_COPYABLE(Swapchain);
        UNDERLYING(VkSwapchainKHR, Swapchain);

        void RecreateSwapchain(const glm::uvec2& size);
        void CreateSwapchain(const glm::uvec2& size, VkSwapchainKHR oldSwapchain = nullptr);
        void CreateImages();

        [[nodiscard]] VkResult AcquireNextImage(VkSemaphore semaphore);
        [[nodiscard]] VkResult Present(VkCommandBuffer cmdBuffer, VkSemaphore waitSemaphore,
                                       VkSemaphore signalSemaphore, VkFence fence);

        [[nodiscard]] uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
        [[nodiscard]] Image& GetCurrentImage() const { return *mImages[mCurrentImageIndex]; }
        [[nodiscard]] VkExtent2D GetExtent() const { return mExtent; }

    private:
        std::shared_ptr<Context> mContext;
        std::shared_ptr<Queue> mGraphicsQueue;

        VkSwapchainKHR mSwapchain{};

        std::array<std::unique_ptr<Image>, 3> mImages;
        uint32_t mCurrentImageIndex = 0;
        VkExtent2D mExtent{};
    };
} // namespace FS::VK
