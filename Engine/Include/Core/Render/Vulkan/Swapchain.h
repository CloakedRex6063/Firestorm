#pragma once

namespace FS::VK
{
    class Device;

    class Swapchain
    {
    public:
        Swapchain(const std::shared_ptr<Device>& device, vk::raii::SurfaceKHR& surface, const glm::uvec2& size);

        ~Swapchain();

        operator vk::raii::SwapchainKHR&() const { return *mSwapchain; }

        void RecreateSwapchain(const glm::uvec2& size);

        void AcquireNextImage(const vk::raii::Semaphore& semaphore, const vk::raii::Fence& fence);

        vk::Image& GetCurrentImage() { return mImages[mCurrentImageIndex]; }
        vk::raii::ImageView& GetCurrentImageView() const { return *mImageViews[mCurrentImageIndex]; }

        [[nodiscard]] uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }

    private:
        std::shared_ptr<Device> mDevice;
        std::unique_ptr<vk::raii::SwapchainKHR> mSwapchain;
        std::unique_ptr<vk::raii::SurfaceKHR> mSurface;
        std::vector<vk::Image> mImages;
        std::vector<std::unique_ptr<vk::raii::ImageView>> mImageViews;
        uint32_t mCurrentImageIndex = 0;
    };
} // namespace FS::VK
