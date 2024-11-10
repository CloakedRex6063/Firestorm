#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    Swapchain::Swapchain(const std::shared_ptr<Device>& device, vk::raii::SurfaceKHR& surface,
                             const glm::uvec2& size) : mDevice(device)
    {
        mSurface = std::make_unique<vk::raii::SurfaceKHR>(std::move(surface));
        mSwapchain = std::make_unique<vk::raii::SwapchainKHR>(mDevice->CreateSwapchainKHR(*mSurface, size));

        mImages = mSwapchain->getImages();
        mImageViews.reserve(mImages.size());
        for (const auto& image : mImages)
        {
            mImageViews.emplace_back
            (
                device->CreateImageView(image, vk::Format::eB8G8R8A8Unorm, vk::ImageAspectFlagBits::eColor)
            );
        }
    }

    Swapchain::~Swapchain()
    {
        mSwapchain.reset();
    }

    void Swapchain::RecreateSwapchain(const glm::uvec2& size)
    {
        mSwapchain = std::make_unique<vk::raii::SwapchainKHR>(mDevice->CreateSwapchainKHR(*mSurface, size, *mSwapchain));

        mImages = mSwapchain->getImages();
        mImageViews.clear();
        mImageViews.reserve(mImages.size());
        for (const auto& image : mImages)
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.setViewType(vk::ImageViewType::e2D);
            createInfo.setImage(image);
            createInfo.setFormat(vk::Format::eB8G8R8A8Unorm);

            vk::ImageSubresourceRange subresourceRange{};
            subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            subresourceRange.setLevelCount(1);
            subresourceRange.setLayerCount(1);
            createInfo.setSubresourceRange(subresourceRange);

            mImageViews.emplace_back
            (
                mDevice->CreateImageView(image, vk::Format::eB8G8R8A8Unorm, vk::ImageAspectFlagBits::eColor)
            );
        }
    }

    void Swapchain::AcquireNextImage(const vk::raii::Semaphore& semaphore, const vk::raii::Fence& fence)
    {
        auto [result, image] = mSwapchain->acquireNextImage(1000000000, semaphore, fence);
        mCurrentImageIndex = image;
        switch (result)
        {
        case vk::Result::eSuccess:
            break;
        default:
            //TODO: Create Swapchain
            break;
        }
    }
} // FS