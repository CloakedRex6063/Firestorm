#pragma once

namespace FS::VK
{
    class Device;
    class Buffer
    {
    public:
        Buffer(const std::shared_ptr<Device>& device, uint32_t allocSize, vk::BufferUsageFlags usage,
               VmaMemoryUsage memoryUsage);
        ~Buffer();
        MOVABLE(Buffer);
        NON_COPYABLE(Buffer);

        operator vk::Buffer&() const { return *mBuffer; }
        [[nodiscard]] VmaAllocation& GetAllocation() const { return *mAllocation; }

    private:
        std::shared_ptr<Device> mDevice;
        
        std::unique_ptr<VmaAllocation> mAllocation;
        VmaAllocationInfo mAllocationInfo{};
        
        std::unique_ptr<vk::Buffer> mBuffer;
    };
} // namespace FS::VK