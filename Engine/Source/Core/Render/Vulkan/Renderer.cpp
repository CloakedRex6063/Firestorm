#include "Core/Render/Vulkan/Renderer.h"
#include "Core/Render/Vulkan/Device.h"
#include "Core/Render/Vulkan/Image.h"
#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Utils.h"
#include "Core/Render/Window.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

namespace FS::VK
{
    Renderer::Renderer()
    {
        mDevice = std::make_unique<Device>(GetWindow());
        auto surface = GetWindow().CreateSurface(mDevice->GetInstance());
        auto size = GetWindow().GetSize();
        mSwapchain = std::make_unique<Swapchain>(mDevice, surface, size);
        mQueue = std::make_unique<Queue>(mDevice, vk::QueueFlagBits::eGraphics);
        auto extent = vk::Extent2D(size.x, size.y);
        auto allocInfo = Utils::ImageAllocInfo(VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        mRenderImage = std::make_unique<Image>(mDevice, extent, vk::Format::eR16G16B16A16Sfloat, allocInfo);
        mSwapchainExtent = extent;
        mGeometryPipeline = std::make_unique<GeometryPipeline>(GetDevice(), GetWindow().GetSize());

        for (auto& frame : mFrameData)
        {
            auto command = Command(mDevice);
            auto renderSemaphore = GetDevice().CreateSemaphore();
            auto presentSemaphore = GetDevice().CreateSemaphore();
            auto fence = GetDevice().CreateFence(vk::FenceCreateFlagBits::eSignaled);
            frame = std::make_unique<FrameData>(command, renderSemaphore, presentSemaphore, fence);
        }
    }

    Renderer::~Renderer() { GetDevice().WaitIdle(); }

    void Renderer::BeginFrame()
    {
        GetWindow().PollEvents();
        auto& [command, renderSemaphore, presentSemaphore, fence] = GetCurrentFrame();
        GetDevice().WaitForFence(fence, 1000000000);
        GetDevice().ResetFence(fence);
        GetSwapchain().AcquireNextImage(renderSemaphore, fence);
    }

    void Renderer::Render()
    {
        auto& [command, renderSemaphore, presentSemaphore, fence] = GetCurrentFrame();
        command.Reset();
        command.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        const auto image = GetSwapchain().GetCurrentImage();

        command.TransitionImage(GetRenderImage(), vk::ImageLayout::eUndefined,
                                vk::ImageLayout::eGeneral);
        command.Clear(GetRenderImage());
        command.BeginRendering(GetRenderImage().GetView(), mSwapchainExtent);
        command.BindPipeline(vk::PipelineBindPoint::eGraphics, GetGeometryPipeline());
        command.SetViewportAndScissor(mSwapchainExtent);
        command.Draw(3);
        command.EndRendering();

        command.TransitionImage(GetRenderImage(), vk::ImageLayout::eGeneral,
                                vk::ImageLayout::eTransferSrcOptimal);
        command.TransitionImage(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        command.CopyImage(GetRenderImage(), image, mSwapchainExtent);

        command.TransitionImage(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);
        command.End();
    }

    void Renderer::EndFrame()
    {
        auto& [command, renderSemaphore, presentSemaphore, fence] = GetCurrentFrame();
        auto commandSubmitInfo = command.GetSubmitInfo();
        auto waitSemaphoreInfo =
            Utils::GetSemaphoreSubmitInfo(vk::PipelineStageFlagBits2::eColorAttachmentOutput, renderSemaphore);
        auto signalSemaphoreInfo =
            Utils::GetSemaphoreSubmitInfo(vk::PipelineStageFlagBits2::eAllGraphics, presentSemaphore);
        GetQueue().SubmitCommand(signalSemaphoreInfo, waitSemaphoreInfo, commandSubmitInfo);
        try
        {
            GetQueue().Present(GetSwapchain(), presentSemaphore);
        }
        catch (vk::SystemError&)
        {
            GetDevice().WaitIdle();
            GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
            const auto size = GetWindow().GetSize();
            auto extent = vk::Extent2D(size.x, size.y);
            auto allocInfo = Utils::ImageAllocInfo(VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            mRenderImage = std::make_unique<Image>(mDevice, extent, vk::Format::eR16G16B16A16Sfloat, allocInfo);
            mSwapchainExtent = extent;
        }
        mFrameIndex = (mFrameIndex + 1) % 3;
    }
} // namespace FS::VK
