#include "Core/Render/Vulkan/Resources/VulkanImage.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanImage::VulkanImage(const std::shared_ptr<VulkanContext>& context, VkImage image, const ImageType type, const VkFormat format)
        : mContext(context), mImage(image)
    {
        mImageView = mContext->CreateImageView(image, type, format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    VulkanImage::VulkanImage(const std::shared_ptr<VulkanContext>& context,
                 const ImageType type,
                 const VkFormat format,
                 const VkExtent2D extent,
                 const VkImageUsageFlags usageFlags,
                 const VkImageAspectFlags aspectFlags)
        : mContext(context)
    {
        std::tie(mImage, mAllocation) = context->CreateImage(type, format, extent, usageFlags);
        mImageView = mContext->CreateImageView(mImage, type, format, aspectFlags);
    }

    VulkanImage::~VulkanImage()
    {
        if (mImageView) vkDestroyImageView(*mContext, mImageView, nullptr);
        if (mAllocation)
        {
            vmaDestroyImage(mContext->GetAllocator(), mImage, mAllocation);
        }
    }
}  // namespace FS