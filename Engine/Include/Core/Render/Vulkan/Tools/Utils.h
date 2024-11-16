#pragma once

namespace FS::VK
{
    class Utils
    {
    public:
        [[nodiscard]] static VkSemaphoreSubmitInfo GetSemaphoreSubmitInfo(VkSemaphore semaphore,
                                                                          VkPipelineStageFlags2 stageMask);
        [[nodiscard]] static VkCommandBufferSubmitInfo GetCommandBufferSubmitInfo(VkCommandBuffer cmdBuffer);

        [[nodiscard]] static VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspectMask);

        [[nodiscard]] static VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stage,
                                                                                   VkShaderModule shaderModule);

        [[nodiscard]] static VkBufferCopy2 GetBufferCopy2(uint64_t size, uint64_t srcOffset, uint64_t dstOffset);
        
        static std::string_view VkResultToString(VkResult result);

        template <typename Lambda>
        static void HandleResult(VkResult result, Lambda onError)
        {
            if (result != VK_SUCCESS)
            {
                Log::Error("{}", VkResultToString(result));
                onError(result);
            }
        }
    };
}  // namespace FS::VK