#include "Core/Render/Vulkan/Resources/Buffer.h"
#include "Core/Render/Vulkan/Context.h"

namespace FS::VK
{
    Buffer::Buffer(const std::shared_ptr<Context>& context, const BufferType type, const uint32_t allocSize) : mContext(context)
    {
        std::tie(mBuffer, mAllocation, mAllocationInfo) = mContext->CreateBuffer(type, allocSize);
    }

    Buffer::~Buffer()
    {
        if (mAllocation) vmaDestroyBuffer(mContext->GetAllocator(), mBuffer, mAllocation);
    }
}  // namespace FS::VK