#pragma once

namespace FS::VK
{
    class Context;
    class Command
    {
    public:
        Command(const std::shared_ptr<Context>& device, VkCommandPool cmdPool, VkCommandBuffer cmd)
            : mDevice(device), mCommandBuffer(cmd), mCommandPool(cmdPool)
        {
        }
        ~Command();
        NON_COPYABLE(Command);
        MOVABLE(Command);
        UNDERLYING(VkCommandBuffer, CommandBuffer)

        VkCommandPool& GetCommandPool() { return mCommandPool; }

        void Reset() const;
        void Begin(VkCommandBufferUsageFlags flags) const;
        void End() const;

        void BeginRendering(const VkImageView& colorImageView, const VkExtent2D& extent) const;
        void EndRendering() const;

        void SetViewportAndScissor(const VkExtent2D& extent) const;

        void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const;
        void BindVertexBuffer(uint32_t bindingOffset,
                              uint32_t bindingCount,
                              ArrayProxy<VkBuffer> buffers,
                              ArrayProxy<uint64_t> offsets) const;
        void BindIndexBuffer(VkBuffer mIndexBuffer, uint64_t offset) const;

        void Draw(uint32_t vertexCount,
                  uint32_t instanceCount = 1,
                  uint32_t vertexOffset = 0,
                  uint32_t instanceOffset = 0) const;
        void DrawIndexed(uint32_t indexCount,
                         uint32_t instanceCount = 1,
                         uint32_t indexOffset = 0,
                         int32_t vertexOffset = 0,
                         uint32_t instanceOffset = 0) const;

        void TransitionImageLayout(VkImage currentImage, VkImageLayout oldLayout, VkImageLayout newLayout) const;

        void CopyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, const ArrayProxy<VkBufferCopy2>& bufferCopy) const;

    private:
        std::shared_ptr<Context> mDevice;
        VkCommandBuffer mCommandBuffer = nullptr;
        VkCommandPool mCommandPool = nullptr;
    };
}  // namespace FS::VK