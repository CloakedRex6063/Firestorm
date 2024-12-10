#include "Core/Render/Vulkan/VulkanCommand.h"

#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"

namespace FS
{
    VulkanCommand::~VulkanCommand()
    {
        if (mDevice)
        {
            vkDestroyCommandPool(*mDevice, mCommandPool, nullptr);
        }
    }
    void VulkanCommand::Reset() const { vkResetCommandBuffer(mCommandBuffer, 0); }

    void VulkanCommand::Begin(const VkCommandBufferUsageFlags flags) const
    {
        const VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = flags};
        vkBeginCommandBuffer(mCommandBuffer, &beginInfo);
    }
    void VulkanCommand::End() const { vkEndCommandBuffer(mCommandBuffer); }

    void VulkanCommand::BeginRendering(const VkImageView colorImageView,
                                       const VkImageView depthImageView,
                                       const VkExtent2D& extent,
                                       const bool clear) const
    {
        VkRenderingAttachmentInfo colorAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = colorImageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = VkClearValue({0.0f, 0.0f, 0.0f, 1.0f}),
        };

        VkRenderingAttachmentInfo depthAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = depthImageView,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = VkClearValue({0.0f, 0.0f, 0.0f, 0.0f}),
        };

        const auto renderArea = VkRect2D(VkOffset2D(0, 0), extent);
        const VkRenderingInfo renderingInfo = {.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                               .renderArea = renderArea,
                                               .layerCount = 1,
                                               .colorAttachmentCount = 1,
                                               .pColorAttachments = &colorAttachment,
                                               .pDepthAttachment = &depthAttachment,
                                               .pStencilAttachment = nullptr};
        vkCmdBeginRendering(mCommandBuffer, &renderingInfo);
        SetViewportAndScissor(extent);
    }

    void VulkanCommand::Draw(const uint32_t vertexCount,
                             const uint32_t instanceCount,
                             const uint32_t vertexOffset,
                             const uint32_t instanceOffset) const
    {
        vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
    }

    void VulkanCommand::DrawIndexed(const uint32_t indexCount,
                                    const uint32_t instanceCount,
                                    const uint32_t indexOffset,
                                    const uint32_t vertexOffset,
                                    const uint32_t instanceOffset) const
    {
        vkCmdDrawIndexed(mCommandBuffer,
                         indexCount,
                         instanceCount,
                         indexOffset,
                         static_cast<int>(vertexOffset),
                         instanceOffset);
    }

    void VulkanCommand::DrawIndexedIndirect(VkBuffer buffer,
                                            const uint64_t offset,
                                            const uint32_t drawCount,
                                            const uint32_t stride) const
    {
        vkCmdDrawIndexedIndirect(mCommandBuffer, buffer, offset, drawCount, stride);
    }
    
    void VulkanCommand::DrawMeshEXT(const uint32_t countX, const uint32_t countY, const uint32_t countZ) const
    {
        vkCmdDrawMeshTasks(mCommandBuffer, countX, countY, countZ);
    }

    void VulkanCommand::EndRendering() const { vkCmdEndRendering(mCommandBuffer); }

    void VulkanCommand::SetViewportAndScissor(const VkExtent2D& extent) const
    {
        const VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 1.f,
            .maxDepth = 0.f,
        };

        const VkRect2D scissor = {.extent = extent};
        vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommand::BindPipeline(const VkPipelineBindPoint bindPoint, VkPipeline pipeline) const
    {
        vkCmdBindPipeline(mCommandBuffer, bindPoint, pipeline);
    }

    void VulkanCommand::BindVertexBuffer(const uint32_t bindingOffset,
                                         const uint32_t bindingCount,
                                         const ArrayProxy<VkBuffer> buffers,
                                         const ArrayProxy<uint64_t> offsets) const
    {
        vkCmdBindVertexBuffers(mCommandBuffer, bindingOffset, bindingCount, buffers.data(), offsets.data());
    }

    void VulkanCommand::BindIndexBuffer(VkBuffer mIndexBuffer, const uint64_t offset) const
    {
        vkCmdBindIndexBuffer(mCommandBuffer, mIndexBuffer, offset, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommand::BindDescriptorSet(const VkPipelineBindPoint bindPoint,
                                          VkPipelineLayout pipelineLayout,
                                          VkDescriptorSet set) const
    {
        vkCmdBindDescriptorSets(mCommandBuffer, bindPoint, pipelineLayout, 0, 1, &set, 0, nullptr);
    }

    void VulkanCommand::SetPushConstants(VkPipelineLayout layout,
                                         const VkShaderStageFlags stageFlags,
                                         const uint32_t size,
                                         const void* data) const
    {
        vkCmdPushConstants(mCommandBuffer, layout, stageFlags, 0, size, data);
    }

    void VulkanCommand::TransitionImageLayout(VkImage currentImage,
                                              const ImageLayout oldLayout,
                                              const ImageLayout newLayout) const
    {
        const auto vkOldLayout = VulkanUtils::GetImageLayout(oldLayout);
        const auto vkNewLayout = VulkanUtils::GetImageLayout(newLayout);

        VkImageMemoryBarrier2 imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            .oldLayout = vkOldLayout,
            .newLayout = vkNewLayout,
            .image = currentImage,
            .subresourceRange = VulkanUtils::GetSubresourceRange(
                newLayout == ImageLayout::eDepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
        };

        const VkDependencyInfo dependencyInfo = {.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                                 .imageMemoryBarrierCount = 1,
                                                 .pImageMemoryBarriers = &imageMemoryBarrier};

        vkCmdPipelineBarrier2(mCommandBuffer, &dependencyInfo);
    }

    void VulkanCommand::CopyBufferToBuffer(VkBuffer srcBuffer,
                                           VkBuffer dstBuffer,
                                           const ArrayProxy<VkBufferCopy2>& bufferCopy) const
    {
        const VkCopyBufferInfo2 copyBufferInfo = {.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                  .srcBuffer = srcBuffer,
                                                  .dstBuffer = dstBuffer,
                                                  .regionCount = bufferCopy.size(),
                                                  .pRegions = bufferCopy.data()};
        vkCmdCopyBuffer2(mCommandBuffer, &copyBufferInfo);
    }

    void VulkanCommand::CopyBufferToImage(VkBuffer srcBuffer,
                                          VkImage dstImage,
                                          const ArrayProxy<VkBufferImageCopy2>& bufferImageCopies) const
    {
        const VkCopyBufferToImageInfo2 copyImageInfo = {
            .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
            .srcBuffer = srcBuffer,
            .dstImage = dstImage,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = bufferImageCopies.size(),
            .pRegions = bufferImageCopies.data(),
        };
        vkCmdCopyBufferToImage2(mCommandBuffer, &copyImageInfo);
    };
}  // namespace FS