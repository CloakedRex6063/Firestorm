#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "Core/Engine.h"
#include "Core/Render/Resources/Model.hpp"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanSwapchain.h"
#include "Core/Render/Vulkan/Pipelines/VulkanGeometryPipeline.h"
#include "Core/Render/Vulkan/Resources/VulkanBuffer.h"
#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/Resources/VulkanImage.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"
#include "Core/Systems/FileSystem.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Core/Render/Vulkan/Resources/VulkanModel.h"
#include "Core/Render/Vulkan/Resources/VulkanResourceLoader.h"

namespace FS
{
    VulkanRenderer::VulkanRenderer()
    {
        auto size = GetWindow().GetSize();

        mContext = std::make_shared<VulkanContext>(GetWindow());
        mSwapchain = std::make_unique<VulkanSwapchain>(mContext, size);
        mModelManager = std::make_unique<VulkanResourceLoader>(mContext);
        mGeometryPipeline = std::make_unique<VulkanGeometryPipeline>(mContext,
                                                                     GetModelManager().GetDescriptor().GetLayout(),
                                                                     size);

        for (auto& frame : mFrameData)
        {
            frame = std::make_unique<FrameData>(GetContext().CreateFence(VK_FENCE_CREATE_SIGNALED_BIT),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateCommand(GetContext().GetGraphicsQueue()));
        }

        mDepthImage = std::make_unique<VulkanImage>(mContext,
                                                    ImageType::e2D,
                                                    VK_FORMAT_D32_SFLOAT,
                                                    GetSwapchain().GetExtent(),
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                    VK_IMAGE_ASPECT_DEPTH_BIT);

        const auto otherModelPath = FS::gEngine.FileSystem().GetPath(FS::Directory::eGameAssets, "Models/DamagedHelmet.glb");
        auto otherModel = FS::gEngine.ResourceSystem().LoadModel(otherModelPath).value();
        auto modelsToUpload = gEngine.ResourceSystem().GetModelsToUpload();
        mModelManager->UploadModels(modelsToUpload);
    }

    VulkanRenderer::~VulkanRenderer() { GetContext().WaitIdle(); }

    void VulkanRenderer::BeginFrame()
    {
        GetWindow().PollEvents();
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        GetContext().WaitForFence(fence, 1000000000);
        GetContext().ResetFence(fence);
        VulkanUtils::HandleResult(GetSwapchain().AcquireNextImage(renderSemaphore),
                                  [&](VkResult)
                                  {
                                      GetContext().WaitIdle();
                                      GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                                      mDepthImage = std::make_unique<VulkanImage>(mContext,
                                                                                  ImageType::e2D,
                                                                                  VK_FORMAT_D32_SFLOAT,
                                                                                  GetSwapchain().GetExtent(),
                                                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                                  VK_IMAGE_ASPECT_DEPTH_BIT);
                                  });
    }

    void VulkanRenderer::Render()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        auto& currentImage = GetSwapchain().GetCurrentImage();

        command.Reset();
        command.Begin(0);

        command.TransitionImageLayout(currentImage, ImageLayout::eUndefined, ImageLayout::eColorAttachment);
        command.TransitionImageLayout(*mDepthImage, ImageLayout::eUndefined, ImageLayout::eDepthAttachment);

        command.BeginRendering(currentImage.GetView(), mDepthImage->GetView(), GetSwapchain().GetExtent());

        RenderGeometry(command);

        command.EndRendering();

        command.TransitionImageLayout(currentImage, ImageLayout::eColorAttachment, ImageLayout::ePresent);

        command.End();
    }

    void VulkanRenderer::EndFrame()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        VulkanUtils::HandleResult(GetSwapchain().Present(command, renderSemaphore, presentSemaphore, fence),
                                  [&](VkResult)
                                  {
                                      GetContext().WaitIdle();
                                      GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                                      mDepthImage = std::make_unique<VulkanImage>(mContext,
                                                                                  ImageType::e2D,
                                                                                  VK_FORMAT_D32_SFLOAT,
                                                                                  GetSwapchain().GetExtent(),
                                                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                                  VK_IMAGE_ASPECT_DEPTH_BIT);
                                  });
        mFrameIndex = (mFrameIndex + 1) % Constants::MaxFramesInFlight;
    }

    static float rotationAngle = 0.0f;

    void VulkanRenderer::RenderGeometry(const VulkanCommand& command) const
    {
        command.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, GetGeometryPipeline());

        const auto size = GetWindow().GetSize();
        const auto aspect = static_cast<float>(size.x) / static_cast<float>(size.y);

        rotationAngle += 0.1f * gEngine.GetDeltaTime();
        constexpr float cameraRadius = 5.0f;

        const auto cameraPosition = glm::vec3(cameraRadius * cos(rotationAngle),
                                              // X-coordinate
                                              0.0f,
                                              // Y-coordinate (fixed height)
                                              cameraRadius * sin(rotationAngle) // Z-coordinate
            );

        auto modelCenter = glm::vec3(0.0f, 0.0f, 0.0f);

        // Compute the view matrix
        const auto view = glm::lookAt(cameraPosition, modelCenter, glm::vec3(0, 1, 0));

        auto projection = glm::perspective(glm::radians(45.0f), aspect, 0.01f, 1000.0f);
        projection[1][1] *= -1;

        for (auto [index, model] : std::views::enumerate(mModelManager->GetModels()))
        {
            command.BindIndexBuffer(model.GetIndexBuffer(), 0);

            command.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      GetGeometryPipeline().GetLayout(),
                                      GetModelManager().GetDescriptor());

            auto parentTransform = glm::mat4(1.0f);
            for (const auto& nodeIndex : model.GetRootNodeIndices())
            {
                auto& node = model.GetNodes()[nodeIndex];
                const auto& mesh = model.GetMeshes()[node.mMeshIndex];
                ModelPushConstant pushConstant{.mMVP = projection * view * (parentTransform * node.mTransform),
                                               .mVertexAddress = model.GetVertexBufferAddress(),
                                               .mMaterialAddress = model.GetMaterialBufferAddress(),
                                               .mMaterialBaseIndex = mesh.mMaterialIndex};
                command.SetPushConstants(GetGeometryPipeline().GetLayout(),
                                         VK_SHADER_STAGE_ALL,
                                         sizeof(ModelPushConstant),
                                         &pushConstant);
                command.DrawIndexed(mesh.mIndexCount, 1, mesh.mIndexOffset, mesh.mVertexOffset);
                for (const auto& childNodeIndex : node.mChildren)
                {
                    auto& childNode = model.GetNodes()[childNodeIndex];
                    const auto& [vertexOffset, indexOffset, indexCount, materialIndex] =
                        model.GetMeshes()[childNode.mMeshIndex];
                    pushConstant.mMVP = projection * view * (parentTransform * node.mTransform * childNode.mTransform);
                    pushConstant.mMaterialBaseIndex = materialIndex;
                    command.SetPushConstants(GetGeometryPipeline().GetLayout(),
                                             VK_SHADER_STAGE_ALL,
                                             sizeof(ModelPushConstant),
                                             &pushConstant);
                    command.DrawIndexed(indexCount, 1, indexOffset, vertexOffset);
                }
            }
        }
    }
} // namespace FS