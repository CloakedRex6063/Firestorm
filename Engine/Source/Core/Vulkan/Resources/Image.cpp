#include "Core/Render/Vulkan/Resources/Image.h"
#include "Core/Render/Vulkan/Context.h"

namespace FS::VK
{
    Image::Image(const std::shared_ptr<Context>& context, VkImage image, const ImageType type, const VkFormat format)
        : mContext(context), mImage(image)
    {
        mImageView = mContext->CreateImageView(image, type, format);
    }
    
    Image::Image(const std::shared_ptr<Context>& context,
                 const ImageType type,
                 const VkFormat format,
                 const VkExtent2D extent,
                 const VkImageUsageFlags usageFlags)
    {
        std::tie(mImage, mAllocation) = context->CreateImage(type, format, extent, usageFlags);
        mImageView = mContext->CreateImageView(mImage, type, format);
    }
    
    Image::~Image()
    {
        if (mImageView) vkDestroyImageView(*mContext, mImageView, nullptr);
        if (mAllocation) vmaDestroyImage(mContext->GetAllocator(), mImage, mAllocation);
    }
}  // namespace FS::VK