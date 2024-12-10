#pragma once
#include "ktxvulkan.h"
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS
{
    class VulkanContext;
    class VulkanImage
    {
    public:
        VulkanImage(const std::shared_ptr<VulkanContext>& context, const std::string& path, ktxVulkanDeviceInfo mVDI);
        VulkanImage(const std::shared_ptr<VulkanContext>& context, VkImage image, ImageType type, VkFormat format);
        VulkanImage(const std::shared_ptr<VulkanContext>& context,
              ImageType type,
              VkFormat format,
              VkExtent2D extent,
              VkImageUsageFlags usageFlags,
              VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

        ~VulkanImage();
        NON_COPYABLE(VulkanImage);

        VulkanImage(VulkanImage&& other) noexcept {
            this->mImage = other.mImage;
            this->mImageView = other.mImageView;
            this->mContext = other.mContext;
            this->mAllocation = other.mAllocation;
            this->mTexture = other.mTexture;

            other.mTexture = {};
            other.mImage = nullptr;
            other.mImageView = nullptr;
            other.mContext = nullptr;
            other.mAllocation = nullptr;
        }

        VulkanImage& operator=(VulkanImage&& other) noexcept {
            if (this != &other) {
                this->mImage = other.mImage;
                this->mImageView = other.mImageView;
                this->mContext = other.mContext;
                this->mAllocation = other.mAllocation;
                this->mTexture = other.mTexture;

                other.mTexture = {};
                other.mImage = nullptr;
                other.mImageView = nullptr;
                other.mContext = nullptr;
                other.mAllocation = nullptr;
            }
            return *this;
        }
        
        UNDERLYING(VkImage, Image);

        VkImageView& GetView() { return mImageView; }
        [[nodiscard]] VmaAllocation& GetAllocation() { return mAllocation; }

    private:
        std::shared_ptr<VulkanContext> mContext;

        ktxVulkanTexture mTexture{};
        VkImage mImage{};
        VkImageView mImageView{};
        VmaAllocation mAllocation{};
    };
}  // namespace FS