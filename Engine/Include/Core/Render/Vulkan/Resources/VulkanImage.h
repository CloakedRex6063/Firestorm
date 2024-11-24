#pragma once
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS
{
    class VulkanContext;
    class VulkanImage
    {
    public:
        VulkanImage(const std::shared_ptr<VulkanContext>& context, VkImage image, ImageType type, VkFormat format);
        VulkanImage(const std::shared_ptr<VulkanContext>& context,
              ImageType type,
              VkFormat format,
              VkExtent2D extent,
              VkImageUsageFlags usageFlags,
              VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

        ~VulkanImage();
        NON_COPYABLE(VulkanImage);
        MOVABLE(VulkanImage);
        UNDERLYING(VkImage, Image);

        VkImageView& GetView() { return mImageView; }

    private:
        std::shared_ptr<VulkanContext> mContext;

        VkImage mImage{};
        VkImageView mImageView{};
        VmaAllocation mAllocation{};
    };
}  // namespace FS