#pragma once
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS::VK
{
    class Context;
    class Image
    {
    public:
        Image(const std::shared_ptr<Context>& context, VkImage image, ImageType type, VkFormat format);
        Image(const std::shared_ptr<Context>& context,
              ImageType type,
              VkFormat format,
              VkExtent2D extent,
              VkImageUsageFlags usageFlags,
              VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

        ~Image();
        NON_COPYABLE(Image);
        MOVABLE(Image);
        UNDERLYING(VkImage, Image);

        VkImageView& GetView() { return mImageView; }

    private:
        std::shared_ptr<Context> mContext;

        VkImage mImage{};
        VkImageView mImageView{};
        VmaAllocation mAllocation{};
    };
}  // namespace FS::VK