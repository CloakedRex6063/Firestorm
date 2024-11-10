#pragma once

namespace FS::VK
{
    class Device;

    class Command
    {
    public:
        Command(const std::shared_ptr<Device>& device);

        void Begin(vk::CommandBufferUsageFlags flags) const;
        void End() const;

        void Reset() const;

        void Clear(const vk::Image& image) const;
        void BeginRendering(vk::ImageView imageView, const vk::Extent2D& extent) const;
        void EndRendering() const;

        void BindPipeline(const vk::PipelineBindPoint& bindPoint, const vk::raii::Pipeline& pipeline) const;

        void SetViewportAndScissor(const vk::Extent2D& size) const;
        void SetViewportAndScissor(const glm::uvec2& size, const vk::Rect2D& scissor) const;

        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
        
        void TransitionImage(vk::Image image, vk::ImageLayout currentImageLayout, vk::ImageLayout nextImageLayout,
                             vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor) const;

        void CopyImage(vk::Image from, vk::Image to, vk::Extent2D extent) const;
        [[nodiscard]] vk::CommandBufferSubmitInfo GetSubmitInfo() const;

        operator vk::raii::CommandBuffer&() const { return *mCommandBuffer; }
        [[nodiscard]] vk::raii::CommandPool& GetPool() const { return *mCommandPool; }

    private:
        std::shared_ptr<Device> mDevice;
        std::unique_ptr<vk::raii::CommandPool> mCommandPool;
        std::unique_ptr<vk::raii::CommandBuffer> mCommandBuffer;
    };

} // namespace FS::VK
