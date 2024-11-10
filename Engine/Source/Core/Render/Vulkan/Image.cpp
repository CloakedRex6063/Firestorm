#include "Core/Render/Vulkan/Image.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    Image::Image(const std::shared_ptr<Device>& device, const vk::Extent2D& extent, const vk::Format& format,
                 const VmaAllocationCreateInfo& allocInfo) : mDevice(device), mExtent(extent), mFormat(format)
    {
        const auto usageFlags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                                vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment;
        mAllocation = std::make_unique<VmaAllocation>();
        mImage = device->CreateImage(mExtent, mFormat, usageFlags, allocInfo, *mAllocation);
        mView = device->CreateImageView(*mImage, mFormat, vk::ImageAspectFlagBits::eColor);
    }
    Image::~Image()
    {
        vmaDestroyImage(mDevice->GetAllocator(), *mImage, *mAllocation);
        mImage.reset();
    }
}; // namespace FS::VK