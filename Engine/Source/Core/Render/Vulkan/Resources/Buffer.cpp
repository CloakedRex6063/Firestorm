#include "Core/Render/Vulkan/Resources/Buffer.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    Buffer::Buffer(const std::shared_ptr<Device>& device, const uint32_t allocSize, const vk::BufferUsageFlags usage,
                   const VmaMemoryUsage memoryUsage)
        : mDevice(device)
    {
        mAllocation = std::make_unique<VmaAllocation>();
        mBuffer = device->CreateBuffer(allocSize, usage, memoryUsage, GetAllocation());
        vmaGetAllocationInfo(device->GetAllocator(), GetAllocation(), &mAllocationInfo);
    }

    Buffer::~Buffer()
    {
        if (mAllocation) vmaDestroyBuffer(mDevice->GetAllocator(), *mBuffer, GetAllocation());
    }
} // namespace FS::VK