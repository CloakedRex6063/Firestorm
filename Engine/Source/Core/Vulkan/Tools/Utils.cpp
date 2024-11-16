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