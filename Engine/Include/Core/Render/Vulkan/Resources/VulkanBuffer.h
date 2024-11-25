#pragma once
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS
{
    class VulkanContext;
    class VulkanBuffer
    {
    public:
        VulkanBuffer(const std::shared_ptr<VulkanContext>& context, BufferType type, uint32_t allocSize);
        ~VulkanBuffer();
        NON_COPYABLE(VulkanBuffer);
        MOVABLE(VulkanBuffer);
        UNDERLYING(VkBuffer, Buffer);

        VmaAllocation& GetAllocation() { return mAllocation; }
        VmaAllocationInfo& GetAllocationInfo() { return mAllocationInfo; }

    private:
        std::shared_ptr<VulkanContext> mContext;

        VkBuffer mBuffer{};
        VmaAllocation mAllocation{};
        VmaAllocationInfo mAllocationInfo{};
    };
}  // namespace FS
