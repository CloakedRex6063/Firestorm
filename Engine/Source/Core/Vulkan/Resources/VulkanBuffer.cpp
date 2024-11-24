#include "Core/Render/Vulkan/Resources/VulkanBuffer.h"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanContext>& context,
                   const BufferType type,
                   const uint32_t allocSize,
                   const VkBufferUsageFlags usageFlags)
        : mContext(context)
    {
        std::tie(mBuffer, mAllocation, mAllocationInfo) = mContext->CreateBuffer(type, allocSize, usageFlags);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (mAllocation) vmaDestroyBuffer(mContext->GetAllocator(), mBuffer, mAllocation);
    }
}  // namespace FS