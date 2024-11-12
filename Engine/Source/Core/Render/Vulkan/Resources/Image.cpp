#include "Core/Render/Vulkan/Resources/Image.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    Image::Image(const std::shared_ptr<Device>& device, const vk::Extent2D& extent, const vk::Format& format,
                 const VmaAllocationCreateInfo& allocInfo)
        : mDevice(device), mExtent(extent), mFormat(format)
    {
        const auto usageFlags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                                vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment;
        mAllocation = std::make_unique<VmaAllocation>();
        mImage = device->CreateImage(mExtent, mFormat, usageFlags, allocInfo, GetAllocation());
        mView = device->CreateImageView(*mImage, mFormat, vk::ImageAspectFlagBits::eColor);
    }

    Image::~Image()
    {
        if (mAllocation) vmaDestroyImage(mDevice->GetAllocator(), *mImage, GetAllocation());
    }
}; // namespace FS::VK