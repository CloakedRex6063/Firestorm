#include "Core/Render/Vulkan/Resources/VulkanImage.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanImage::VulkanImage(const std::shared_ptr<VulkanContext>& context, const std::string& path, ktxVulkanDeviceInfo mVDI)
        : mContext(context)
    {
        ktxTexture* kTexture;
        auto result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);
        result = ktxTexture_VkUploadEx(kTexture,
                                       &mVDI,
                                       &mTexture,
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_SAMPLED_BIT,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        ktxTexture_Destroy(kTexture);
        mImageView =
            mContext->CreateImageView(mTexture.image, ImageType::e2D, VK_FORMAT_BC7_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    VulkanImage::VulkanImage(const std::shared_ptr<VulkanContext>& context,
                             VkImage image,
                             const ImageType type,
                             const VkFormat format)
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
        if (mTexture.image)
        {
            ktxVulkanTexture_Destruct(&mTexture, mContext->GetDevice(), nullptr);
        }
        if (mImageView) vkDestroyImageView(*mContext, mImageView, nullptr);
        if (mAllocation)
        {
            vmaDestroyImage(mContext->GetAllocator(), mImage, mAllocation);
        }
    }
}  // namespace FS