#include "Core/Render/Vulkan/Command.h"

#include "Core/Render/Vulkan/Device.h"
#include "Core/Render/Vulkan/Utils.h"

#include <Core/EnginePCH.hpp>

namespace FS::VK
{
    Command::Command(const std::shared_ptr<Device>& device) : mDevice(device)
    {
        mCommandPool = mDevice->CreateCommandPool(mDevice->GetQueueFamily(vk::QueueFlagBits::eGraphics));
        mCommandBuffer = mDevice->CreateCommandBuffer(GetPool());
    }

    void Command::Begin(const vk::CommandBufferUsageFlags flags) const
    {
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.setFlags(flags);
        mCommandBuffer->begin(beginInfo);
    }

    void Command::End() const { mCommandBuffer->end(); }

    void Command::Reset() const { mCommandBuffer->reset(); }

    void Command::Clear(const vk::Image& image) const
    {
        constexpr auto clearColor = vk::ClearColorValue(std::array{0.0f, 0.0f, 0.0f, 1.0f});
        const auto range = Utils::GetImageSubresourceRange(vk::ImageAspectFlagBits::eColor);
        mCommandBuffer->clearColorImage(image, vk::ImageLayout::eColorAttachmentOptimal, clearColor, range);
    }

    void Command::BeginRendering(const vk::ImageView imageView, const vk::Extent2D& extent) const
    {
        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.setImageView(imageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(vk::ClearValue(vk::ClearColorValue(std::array{0.0f, 0.0f, 0.0f, 1.0f})));

        vk::RenderingInfo renderInfo;
        renderInfo.setRenderArea(vk::Rect2D(vk::Offset2D(), extent))
            .setLayerCount(1)
            .setColorAttachments(colorAttachment);

        mCommandBuffer->beginRendering(renderInfo);
    }

    void Command::EndRendering() const { mCommandBuffer->endRendering(); }

    void Command::BindPipeline(const vk::PipelineBindPoint& bindPoint, const vk::raii::Pipeline& pipeline) const
    {
        mCommandBuffer->bindPipeline(bindPoint, pipeline);
    }

    void Command::SetViewportAndScissor(const vk::Extent2D& size) const
    {
        const auto viewport = vk::Viewport()
                                  .setWidth(static_cast<float>(size.width))
                                  .setHeight(static_cast<float>(size.height))
                                  .setMinDepth(0.0)
                                  .setMaxDepth(1.0);
        mCommandBuffer->setViewport(0, viewport);

        const auto scissor = vk::Rect2D().setExtent(vk::Extent2D(size.width, size.height));
        mCommandBuffer->setScissor(0, scissor);
    }

    void Command::SetViewportAndScissor(const vk::Extent2D& size, const vk::Rect2D& scissor) const
    {
        const auto viewport = vk::Viewport()
                                  .setWidth(static_cast<float>(size.width))
                                  .setHeight(static_cast<float>(size.height))
                                  .setMinDepth(0.0)
                                  .setMaxDepth(1.0);
        mCommandBuffer->setViewport(0, viewport);
        mCommandBuffer->setScissor(0, scissor);
    }
    
    void Command::Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                       const uint32_t firstInstance) const
    {
        mCommandBuffer->draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void Command::TransitionImage(const vk::Image image, const vk::ImageLayout oldLayout,
                                  const vk::ImageLayout newLayout, const vk::ImageAspectFlags aspectMask) const
    {
        vk::ImageMemoryBarrier2 imageMemoryBarrier{};
        imageMemoryBarrier.setOldLayout(oldLayout);
        imageMemoryBarrier.setNewLayout(newLayout);

        imageMemoryBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands);
        imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite);
        imageMemoryBarrier.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands);
        imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead);

        const auto subresourceRange = Utils::GetImageSubresourceRange(aspectMask);
        imageMemoryBarrier.setSubresourceRange(subresourceRange);
        imageMemoryBarrier.setImage(image);

        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.setImageMemoryBarrierCount(1);
        dependencyInfo.setImageMemoryBarriers(imageMemoryBarrier);

        mCommandBuffer->pipelineBarrier2(dependencyInfo);
    }

    void Command::CopyImage(const vk::Image from, const vk::Image to, const vk::Extent2D extent) const
    {
        vk::ImageSubresourceLayers imageSubresource{};
        imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1);

        vk::ImageBlit2 copyImage{};
        copyImage.srcOffsets[1] = vk::Offset3D(static_cast<int>(extent.width), static_cast<int>(extent.height), 1);
        copyImage.dstOffsets[1] = vk::Offset3D(static_cast<int>(extent.width), static_cast<int>(extent.height), 1);
        copyImage.setSrcSubresource(imageSubresource).setDstSubresource(imageSubresource);

        vk::BlitImageInfo2 blitImageInfo{};
        blitImageInfo.setSrcImage(from)
            .setDstImage(to)
            .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
            .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
            .setFilter(vk::Filter::eLinear)
            .setRegions(copyImage)
            .setRegionCount(1);

        mCommandBuffer->blitImage2(blitImageInfo);
    }
    
    void Command::CopyBuffer(const vk::Buffer from, const vk::Buffer to, const vk::BufferCopy2& copyRegion) const
    {
        const auto copyBufferInfo = vk::CopyBufferInfo2().setSrcBuffer(from).setDstBuffer(to).setRegions(copyRegion);
        mCommandBuffer->copyBuffer2(copyBufferInfo);
    }

    vk::CommandBufferSubmitInfo Command::GetSubmitInfo() const
    {
        vk::CommandBufferSubmitInfo submitInfo{};
        submitInfo.setCommandBuffer(*mCommandBuffer);
        submitInfo.setDeviceMask(1);
        return submitInfo;
    }
} // namespace FS::VK
