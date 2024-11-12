#include "Core/Render/Vulkan/Renderer.h"
#include "Core/Render/Vulkan/Device.h"
#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Resources/Image.h"
#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Utils.h"
#include "Core/Render/Window.h"

#define VMA_IMPLEMENTATION
#include "Core/Engine.h"
#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/Render/Vulkan/Resources/ModelManager.h"
#include "Core/ResourceSystem.h"
#include "vma/vk_mem_alloc.h"

namespace FS::VK
{
    Renderer::Renderer()
    {
        mDevice = std::make_unique<Device>(GetWindow());
        auto surface = GetWindow().CreateSurface(mDevice->GetInstance());
        auto size = GetWindow().GetSize();
        mSwapchain = std::make_unique<Swapchain>(mDevice, surface, size);
        mGraphicsQueue = std::make_shared<Queue>(mDevice, vk::QueueFlagBits::eGraphics);
        mTransferQueue = std::make_shared<Queue>(mDevice, vk::QueueFlagBits::eTransfer);
        mModelManager = std::make_unique<ModelManager>(mDevice, mTransferQueue);
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
            auto fence = GetDevice().CreateFence();
            frame = std::make_unique<FrameData>(std::move(command), std::move(renderSemaphore),
                                                std::move(presentSemaphore), std::move(fence));
        }
    }

    Renderer::~Renderer() { GetDevice().WaitIdle(); }

    void Renderer::BeginFrame()
    {
        GetWindow().PollEvents();
        auto& [command, renderSemaphore, presentSemaphore, fence] = GetCurrentFrame();
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
                                vk::ImageLayout::eColorAttachmentOptimal);

        RenderGeometry();

        command.TransitionImage(GetRenderImage(), vk::ImageLayout::eColorAttachmentOptimal,
                                vk::ImageLayout::eTransferSrcOptimal);
        command.TransitionImage(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        command.CopyImage(GetRenderImage(), image, mSwapchainExtent);

        command.TransitionImage(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);
        command.End();
    }

    void Renderer::EndFrame()
    {
        auto& [command, renderSemaphore, presentSemaphore, fence] = GetCurrentFrame();
        const auto commandSubmitInfo = command.GetSubmitInfo();
        const auto waitSemaphoreInfo =
            Utils::GetSemaphoreSubmitInfo(vk::PipelineStageFlagBits2::eColorAttachmentOutput, renderSemaphore);
        const auto signalSemaphoreInfo =
            Utils::GetSemaphoreSubmitInfo(vk::PipelineStageFlagBits2::eAllGraphics, presentSemaphore);
        GetGraphicsQueue().SubmitCommand(signalSemaphoreInfo, waitSemaphoreInfo, commandSubmitInfo);
        try
        {
            GetGraphicsQueue().Present(GetSwapchain(), presentSemaphore);
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
        GetDevice().WaitForFence(fence, 1000000000);
        mFrameIndex = (mFrameIndex + 1) % 3;

        auto& models = GetModelManager().GetModels();
        
        for (auto& id : gEngine.ResourceSystem().GetModelsToRelease())
        {
            models.erase(id);
        }
    }

    void Renderer::RenderGeometry() const
    {
        const auto& command = GetCurrentFrame().mCommand;
        command.BeginRendering(GetRenderImage().GetView(), mSwapchainExtent);
        command.SetViewportAndScissor(mSwapchainExtent);
        command.BindPipeline(vk::PipelineBindPoint::eGraphics, GetGeometryPipeline());

        constexpr std::array<MeshPushConstants, 1> pushConstants = {};
        command.SetPushConstants<MeshPushConstants>(GetGeometryPipeline().GetLayout(), vk::ShaderStageFlagBits::eVertex,
                                                    pushConstants);
        command.Draw(3);
        command.EndRendering();
    }
} // namespace FS::VK
