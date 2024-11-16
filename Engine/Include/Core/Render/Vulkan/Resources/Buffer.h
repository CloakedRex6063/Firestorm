#pragma once
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS::VK
{
    class Context;
    class Buffer
    {
    public:
        Buffer(const std::shared_ptr<Context>& context, BufferType type, uint32_t allocSize);
        ~Buffer();
        NON_COPYABLE(Buffer);
        MOVABLE(Buffer);
        UNDERLYING(VkBuffer, Buffer);

        VmaAllocation& GetAllocation() {return mAllocation;}
        VmaAllocationInfo& GetAllocationInfo() {return mAllocationInfo;}

    private:
        std::shared_ptr<Context> mContext;

        VkBuffer mBuffer{};
        VmaAllocation mAllocation{};
        VmaAllocationInfo mAllocationInfo{};
    };
}  // namespace FS::VK
