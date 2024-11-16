#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Context.h"
#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Render/Vulkan/Renderer.h"
#include "Core/Render/Vulkan/Resources/Image.h"
#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Tools/Utils.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Core/Render/Resources/Model.hpp>

#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Resources/Buffer.h"
#include "stb_image.h"

namespace FS::VK
{
    Renderer::Renderer()
    {
        auto size = GetWindow().GetSize();

        mContext = std::make_shared<Context>(GetWindow());
        mSwapchain = std::make_unique<Swapchain>(mContext, size);
        mGeometryPipeline = std::make_unique<GeometryPipeline>(mContext, size);

        for (auto& frame : mFrameData)
        {
            frame = std::make_unique<FrameData>(GetContext().CreateFence(VK_FENCE_CREATE_SIGNALED_BIT),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateCommand(GetContext().GetGraphicsQueue()));
        }

        mVertices = {
            Vertex{.mPosition = glm::vec3(-0.5, -0.5, 1), .mColor = glm::vec4(1, 0, 0, 1)},
            Vertex{.mPosition = glm::vec3(0.5, -0.5, 1), .mColor = glm::vec4(0, 1, 0, 1)},
            Vertex{.mPosition = glm::vec3(0.5, 0.5, 1), .mColor = glm::vec4(0, 0, 1, 1)},
            Vertex{.mPosition = glm::vec3(-0.5, 0.5, 1), .mColor = glm::vec4(1, 1, 1, 1)},
        };
        mIndices = {0, 1, 2, 2, 3, 0};
        const auto vertexBufferSize = mVertices.size() * sizeof(Vertex);
        const auto indexBufferSize = mIndices.size() * sizeof(uint32_t);

        const auto transferCommand = Command(mContext->CreateCommand(mContext->GetTransferQueue()));
        auto stagingBuffer = Buffer(mContext, BufferType::eStaging, vertexBufferSize + indexBufferSize);
        const auto mapped = mContext->MapMemory(stagingBuffer.GetAllocation());
        std::memcpy(mapped, mVertices.data(), vertexBufferSize);
        std::memcpy((char*)mapped + vertexBufferSize, mIndices.data(), indexBufferSize);
        mContext->UnmapMemory(stagingBuffer.GetAllocation());
        mVertexBuffer = std::make_unique<Buffer>(mContext, BufferType::eVertex, vertexBufferSize);
        mIndexBuffer = std::make_unique<Buffer>(mContext, BufferType::eIndex, vertexBufferSize);

        const auto vertexBufferCopy = Utils::GetBufferCopy2(vertexBufferSize, 0, 0);
        const auto indexBufferCopy = Utils::GetBufferCopy2(indexBufferSize, vertexBufferSize, 0);
        transferCommand.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        transferCommand.CopyBufferToBuffer(stagingBuffer, *mVertexBuffer, vertexBufferCopy);
        transferCommand.CopyBufferToBuffer(stagingBuffer, *mIndexBuffer, indexBufferCopy);
        transferCommand.End();
        const auto fence = GetContext().CreateFence();
        mContext->GetTransferQueue().SubmitQueue(transferCommand, nullptr, 0, nullptr, 0, fence);
        mContext->WaitForFence(fence, UINT64_MAX);
    }

    Renderer::~Renderer() { GetContext().WaitIdle(); }

    void Renderer::BeginFrame()
    {
        GetWindow().PollEvents();
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        GetContext().WaitForFence(fence, 1000000000);
        GetContext().ResetFence(fence);
        Utils::HandleResult(GetSwapchain().AcquireNextImage(renderSemaphore),
                            [&](VkResult)
                            {
                                mContext->WaitIdle();
                                GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                            });
    }

    void Renderer::Render()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        auto& currentImage = GetSwapchain().GetCurrentImage();

        command.Reset();

        command.Begin(0);

        command.TransitionImageLayout(currentImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        command.BeginRendering(currentImage.GetView(), GetSwapchain().GetExtent());

        RenderGeometry(command);

        command.TransitionImageLayout(currentImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        command.End();
    }

    void Renderer::EndFrame()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        Utils::HandleResult(GetSwapchain().Present(command, renderSemaphore, presentSemaphore, fence),
                            [&](VkResult)
                            {
                                mContext->WaitIdle();
                                GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                            });
        mFrameIndex = (mFrameIndex + 1) % 3;
    }

    void Renderer::RenderGeometry(const Command& command) const
    {
        command.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, GetGeometryPipeline());

        command.BindVertexBuffer(0, 1, static_cast<VkBuffer>(*mVertexBuffer), 0ull);
        command.BindIndexBuffer(*mIndexBuffer, 0ull);

        command.DrawIndexed(mIndices.size());

        command.EndRendering();
    }
}  // namespace FS::VK