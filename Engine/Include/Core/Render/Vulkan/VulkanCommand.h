#pragma once
#include "Tools/Enums.hpp"

namespace FS
{
    class VulkanContext;
    class VulkanCommand
    {
    public:
        VulkanCommand(const std::shared_ptr<VulkanContext>& device, VkCommandPool cmdPool, VkCommandBuffer cmd)
            : mDevice(device), mCommandBuffer(cmd), mCommandPool(cmdPool)
        {
        }
        ~VulkanCommand();
        NON_COPYABLE(VulkanCommand);
        MOVABLE(VulkanCommand);
        UNDERLYING(VkCommandBuffer, CommandBuffer)

        VkCommandPool& GetCommandPool() { return mCommandPool; }

        void Reset() const;
        void Begin(VkCommandBufferUsageFlags flags) const;
        void End() const;

        void BeginRendering(VkImageView colorImageView,
                            VkImageView depthImageView,
                            const VkExtent2D& extent,
                            bool clear = true) const;
        void EndRendering() const;

        void SetViewportAndScissor(const VkExtent2D& extent) const;
        void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const;
        void BindVertexBuffer(uint32_t bindingOffset,
                              uint32_t bindingCount,
                              ArrayProxy<VkBuffer> buffers,
                              ArrayProxy<uint64_t> offsets) const;
        void BindIndexBuffer(VkBuffer mIndexBuffer, uint64_t offset) const;
        void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, VkDescriptorSet set) const;
        void SetPushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t size, const void* data) const;

        void Draw(uint32_t vertexCount,
                  uint32_t instanceCount = 1,
                  uint32_t vertexOffset = 0,
                  uint32_t instanceOffset = 0) const;
        void DrawIndexed(uint32_t indexCount,
                         uint32_t instanceCount = 1,
                         uint32_t indexOffset = 0,
                         uint32_t vertexOffset = 0,
                         uint32_t instanceOffset = 0) const;
        void DrawIndexedIndirect(VkBuffer buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) const;
        void DrawMeshEXT(uint32_t countX, uint32_t countY, uint32_t countZ) const;

        void TransitionImageLayout(VkImage currentImage, ImageLayout oldLayout, ImageLayout newLayout) const;

        void CopyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, const ArrayProxy<VkBufferCopy2>& bufferCopy) const;
        void CopyBufferToImage(VkBuffer srcBuffer,
                               VkImage dstImage,
                               const ArrayProxy<VkBufferImageCopy2>& bufferImageCopies) const;

    private:
        std::shared_ptr<VulkanContext> mDevice;
        VkCommandBuffer mCommandBuffer = nullptr;
        VkCommandPool mCommandPool = nullptr;
    };

}  // namespace FS