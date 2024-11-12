#pragma once

namespace FS::VK
{
    class Device;
    class Image
    {
    public:
        Image(const std::shared_ptr<Device>& device, const vk::Extent2D& extent, const vk::Format& format,
              const VmaAllocationCreateInfo& allocInfo);
        ~Image();

        NON_COPYABLE(Image);
        MOVABLE(Image);
        
        operator vk::Image&() const { return *mImage; }
        [[nodiscard]] vk::raii::ImageView& GetView() const { return *mView; }
        [[nodiscard]] vk::Extent2D GetExtent() const { return mExtent; }
        [[nodiscard]] vk::Format GetFormat() const { return mFormat; }
        [[nodiscard]] VmaAllocation& GetAllocation() const { return *mAllocation; }

    private:
        std::shared_ptr<Device> mDevice;
        std::unique_ptr<VmaAllocation> mAllocation;

        std::unique_ptr<vk::Image> mImage;
        std::unique_ptr<vk::raii::ImageView> mView;

        vk::Extent2D mExtent;
        vk::Format mFormat;
    };
} // namespace FS::VK
