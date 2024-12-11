#pragma once
#include "Core/Render/Vulkan/Tools/Enums.hpp"

namespace FS
{
    class VulkanUtils
    {
    public:
        [[nodiscard]] static VkSemaphoreSubmitInfo GetSemaphoreSubmitInfo(VkSemaphore semaphore,
                                                                          VkPipelineStageFlags2 stageMask);
        [[nodiscard]] static VkCommandBufferSubmitInfo GetCommandBufferSubmitInfo(VkCommandBuffer cmdBuffer);

        [[nodiscard]] static VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspectMask);
        [[nodiscard]] static VkImageSubresourceLayers GetSubresourceLayers(VkImageAspectFlags aspectMask);

        [[nodiscard]] static VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stage,
                                                                                   VkShaderModule shaderModule);

        [[nodiscard]] static VkBufferCopy2 GetBufferCopy2(uint64_t size, uint64_t srcOffset, uint64_t dstOffset);
        [[nodiscard]] static VkBufferImageCopy2 GetBufferImageCopy2(uint64_t bufferOffset,
                                                                    VkOffset2D imageOffset,
                                                                    VkExtent2D extent);
        [[nodiscard]] static VkImageCopy2 GetImageCopy2(VkExtent2D extent,
                                                        VkOffset2D srcOffset = VkOffset2D(),
                                                        VkOffset2D dstOffset = VkOffset2D());
        [[nodiscard]] static VkImageBlit2 GetImageBlit2(VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
                                                        const std::array<VkOffset3D, 2>& srcOffsets = {VkOffset3D{0, 0, 0},
                                                                                                       VkOffset3D{0, 0, 1}},
                                                        const std::array<VkOffset3D, 2>& dstOffsets = {VkOffset3D{0, 0, 0},
                                                                                                       VkOffset3D{0, 0, 1}});

        [[nodiscard]] static VkImageLayout GetImageLayout(ImageLayout layout);

        [[nodiscard]] static std::string_view VkResultToString(VkResult result);

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
}  // namespace FS