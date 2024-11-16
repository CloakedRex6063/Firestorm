#include "Core/Render/Vulkan/Command.h"
#include "Core/Render/Vulkan/Context.h"
#include "Core/Render/Vulkan/Tools/Utils.h"

namespace FS::VK
{
    Command::~Command()
    {
        if (mDevice)
        {
            vkDestroyCommandPool(*mDevice, mCommandPool, nullptr);
        }
    }
    void Command::Reset() const { vkResetCommandBuffer(mCommandBuffer, 0); }

    void Command::Begin(const VkCommandBufferUsageFlags flags) const
    {
        const VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = flags};
        vkBeginCommandBuffer(mCommandBuffer, &beginInfo);
    }
    void Command::End() const { vkEndCommandBuffer(mCommandBuffer); }

    void Command::BeginRendering(const VkImageView& colorImageView, const VkExtent2D& extent) const
    {
        VkRenderingAttachmentInfo colorAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = colorImageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = VkClearValue({0.0f, 0.0f, 0.0f, 1.0f}),
        };
        const auto renderArea = VkRect2D(VkOffset2D(0, 0), extent);
        const VkRenderingInfo renderingInfo = {.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                               .renderArea = renderArea,
                                               .layerCount = 1,
                                               .colorAttachmentCount = 1,
                                               .pColorAttachments = &colorAttachment,
                                               .pDepthAttachment = nullptr,
                                               .pStencilAttachment = nullptr};
        vkCmdBeginRendering(mCommandBuffer, &renderingInfo);
        SetViewportAndScissor(extent);
    }

    void Command::Draw(const uint32_t vertexCount,
                       const uint32_t instanceCount,
                       const uint32_t vertexOffset,
                       const uint32_t instanceOffset) const
    {
        vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
    }

    void Command::DrawIndexed(const uint32_t indexCount,
                              const uint32_t instanceCount,
                              const uint32_t indexOffset,
                              const int32_t vertexOffset,
                              const uint32_t instanceOffset) const
    {
        vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
    }

    void Command::EndRendering() const { vkCmdEndRendering(mCommandBuffer); }

    void Command::SetViewportAndScissor(const VkExtent2D& extent) const
    {
        const VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };

        const VkRect2D scissor = {.extent = extent};
        vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
    }

    void Command::BindPipeline(const VkPipelineBindPoint bindPoint, VkPipeline pipeline) const
    {
        vkCmdBindPipeline(mCommandBuffer, bindPoint, pipeline);
    }

    void Command::BindVertexBuffer(const uint32_t bindingOffset,
                                   const uint32_t bindingCount,
                                   const ArrayProxy<VkBuffer> buffers,
                                   const ArrayProxy<uint64_t> offsets) const
    {
        vkCmdBindVertexBuffers(mCommandBuffer, bindingOffset, bindingCount, buffers.data(), offsets.data());
    }

    void Command::BindIndexBuffer(VkBuffer mIndexBuffer, const uint64_t offset) const
    {
        vkCmdBindIndexBuffer(mCommandBuffer, mIndexBuffer, offset, VK_INDEX_TYPE_UINT32);
    }

    void Command::TransitionImageLayout(VkImage currentImage,
                                        const VkImageLayout oldLayout,
                                        const VkImageLayout newLayout) const
    {
        VkImageMemoryBarrier2 imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .image = currentImage,
            .subresourceRange = Utils::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT),
        };

        const VkDependencyInfo dependencyInfo = {.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                                 .imageMemoryBarrierCount = 1,
                                                 .pImageMemoryBarriers = &imageMemoryBarrier};

        vkCmdPipelineBarrier2(mCommandBuffer, &dependencyInfo);
    }

    void Command::CopyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, const ArrayProxy<VkBufferCopy2>& bufferCopy) const
    {
        const VkCopyBufferInfo2 copyBufferInfo = {.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                  .srcBuffer = srcBuffer,
                                                  .dstBuffer = dstBuffer,
                                                  .regionCount = bufferCopy.size(),
                                                  .pRegions = bufferCopy.data()};
        vkCmdCopyBuffer2(mCommandBuffer, &copyBufferInfo);
    };
}  // namespace FS::VK