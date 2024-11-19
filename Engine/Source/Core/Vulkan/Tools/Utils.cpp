#include "Core/Render/Vulkan/Tools/Utils.h"

namespace FS::VK
{
    VkSemaphoreSubmitInfo Utils::GetSemaphoreSubmitInfo(VkSemaphore semaphore, const VkPipelineStageFlags2 stageMask)
    {
        const VkSemaphoreSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                                  .semaphore = semaphore,
                                                  .stageMask = stageMask,
                                                  .deviceIndex = 1};
        return submitInfo;
    }

    VkCommandBufferSubmitInfo Utils::GetCommandBufferSubmitInfo(VkCommandBuffer cmdBuffer)
    {
        const VkCommandBufferSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                      .commandBuffer = cmdBuffer,
                                                      .deviceMask = 1};
        return submitInfo;
    }

    VkImageSubresourceRange Utils::GetSubresourceRange(const VkImageAspectFlags aspectMask)
    {
        const VkImageSubresourceRange subresourceRange = {.aspectMask = aspectMask, .levelCount = 1, .layerCount = 1};
        return subresourceRange;
    }

    VkImageSubresourceLayers Utils::GetSubresourceLayers(const VkImageAspectFlags aspectMask)
    {
        const VkImageSubresourceLayers subresourceLayers = {.aspectMask = aspectMask, .layerCount = 1};
        return subresourceLayers;
    };

    VkPipelineShaderStageCreateInfo Utils::CreateShaderStageInfo(const VkShaderStageFlagBits stage, VkShaderModule shaderModule)
    {
        const VkPipelineShaderStageCreateInfo shaderStageInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                 .stage = stage,
                                                                 .module = shaderModule,
                                                                 .pName = "main"};
        return shaderStageInfo;
    }

    VkBufferCopy2 Utils::GetBufferCopy2(const uint64_t size, const uint64_t srcOffset, const uint64_t dstOffset)
    {
        const VkBufferCopy2 copy2 = {.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                                     .srcOffset = srcOffset,
                                     .dstOffset = dstOffset,
                                     .size = size};
        return copy2;
    }

    VkBufferImageCopy2 Utils::GetBufferImageCopy2(const uint64_t bufferOffset,
                                                  const VkOffset2D imageOffset,
                                                  const VkExtent2D extent)
    {
        const VkBufferImageCopy2 copy2 = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
            .bufferOffset = bufferOffset,
            .imageSubresource = GetSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT),
            .imageOffset = VkOffset3D(imageOffset.x, imageOffset.y, 0),
            .imageExtent = VkExtent3D(extent.width, extent.height, 1),
        };
        return copy2;
    }
    
    VkImageLayout Utils::GetImageLayout(const ImageLayout layout)
    {
        switch (layout)
        {
            case ImageLayout::eUndefined:
                return VK_IMAGE_LAYOUT_UNDEFINED;
            case ImageLayout::eGeneral:
                return VK_IMAGE_LAYOUT_GENERAL;
            case ImageLayout::eColorAttachment:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case ImageLayout::eDepthStencilAttachment:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageLayout::eDepthStencilReadOnly:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case ImageLayout::eShaderReadOnly:
                return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case ImageLayout::eTransferSrc:
                return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case ImageLayout::eTransferDst:
                return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case ImageLayout::ePreInitialised:
                return VK_IMAGE_LAYOUT_PREINITIALIZED;
            case ImageLayout::eDepthReadOnlyStencilAttachment:
                return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageLayout::eDepthAttachmentStencilReadOnly:
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
            case ImageLayout::eDepthAttachment:
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case ImageLayout::eDepthReadOnly:
                return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            case ImageLayout::eStencilAttachment:
                return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageLayout::eStencilReadOnly:
                return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
            case ImageLayout::eReadOnly:
                return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            case ImageLayout::eAttachment:
                return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            case ImageLayout::ePresent:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    std::string_view Utils::VkResultToString(const VkResult result)
    {
        switch (result)
        {
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_NOT_READY:
                return "VK_NOT_READY";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_TIMEOUT:
                return "VK_TIMEOUT";
            case VK_EVENT_SET:
                return "VK_EVENT_SET";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            default:
                return "UNKNOWN_ERROR";
        }
    }

}  // namespace FS::VK