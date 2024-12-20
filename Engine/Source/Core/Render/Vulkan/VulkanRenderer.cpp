#include "Core/Render/Vulkan/VulkanRenderer.h"
#include "imgui.h"
#include "Core/ECS.h"
#include "Core/Engine.h"
#include "Core/Context.h"
#include "Core/Render/Resources/Model.hpp"
#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/VulkanSwapchain.h"
#include "Core/Render/Vulkan/Pipelines/VulkanGeometryPipeline.h"
#include "Core/Render/Vulkan/Resources/VulkanBuffer.h"
#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/Resources/VulkanImage.h"
#include "Core/Render/Vulkan/Resources/VulkanModel.h"
#include "Core/Render/Vulkan/Resources/VulkanResourceLoader.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"
#include "Systems/CameraSystem.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/FileIO.h"
#include "Core/Render/Vulkan/Pipelines/VulkanMeshPipeline.h"
#include "ktxvulkan.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include <Tools/EnginePCH.hpp>
#include <Tools/EnginePCH.hpp>

namespace FS
{
    VulkanRenderer::VulkanRenderer()
    {
        auto size = gEngine.Context().GetWindowSize();

        mContext = std::make_shared<VulkanContext>();
        mSwapchain = std::make_unique<VulkanSwapchain>(mContext, size);
        mResourceLoader = std::make_unique<VulkanResourceLoader>(mContext);
        mGeometryPipeline =
            std::make_unique<VulkanGeometryPipeline>(mContext, GetResourceLoader().GetDescriptor().GetLayout(), size);
        mMeshPipeline = std::make_unique<VulkanMeshPipeline>(mContext, GetResourceLoader().GetDescriptor().GetLayout(), size);

        for (auto& frame : mFrameData)
        {
            frame = std::make_unique<FrameData>(GetContext().CreateFence(VK_FENCE_CREATE_SIGNALED_BIT),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateCommand(GetContext().GetGraphicsQueue()));
        }

        mRenderImage = std::make_unique<VulkanImage>(mContext,
                                                     ImageType::e2D,
                                                     VK_FORMAT_R16G16B16A16_SFLOAT,
                                                     GetSwapchain().GetExtent(),
                                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                                     VK_IMAGE_ASPECT_COLOR_BIT);

        mDepthImage = std::make_unique<VulkanImage>(mContext,
                                                    ImageType::e2D,
                                                    VK_FORMAT_D32_SFLOAT,
                                                    GetSwapchain().GetExtent(),
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                    VK_IMAGE_ASPECT_DEPTH_BIT);

        mUniformBuffer =
            std::make_unique<VulkanBuffer>(mContext, BufferType::eMappedUniform, static_cast<uint32_t>(sizeof(mUBO)));
        mappedBuffer = mContext->MapMemory(mUniformBuffer->GetAllocation());
        mContext->UpdateDescriptorUniformBuffer(*mUniformBuffer, GetResourceLoader().GetDescriptor().GetSet(), 0);

        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForVulkan(&static_cast<SDL_Window&>(gEngine.Context()));

        constexpr std::array poolSizes = {VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                          VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

        mImGuiDescriptorPool = mContext->CreateDescriptorPool(1000, poolSizes);

        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        ImGui_ImplVulkan_InitInfo imGuiInitInfo{.Instance = mContext->GetInstance(),
                                                .PhysicalDevice = mContext->GetPhysicalDevice(),
                                                .Device = GetContext(),
                                                .QueueFamily = mContext->GetGraphicsQueue().GetFamilyIndex(),
                                                .Queue = mContext->GetGraphicsQueue(),
                                                .DescriptorPool = mImGuiDescriptorPool,
                                                .MinImageCount = 3,
                                                .ImageCount = 3,
                                                .UseDynamicRendering = true,
                                                .PipelineRenderingCreateInfo = {
                                                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                                                    .colorAttachmentCount = 1,
                                                    .pColorAttachmentFormats = &colorFormat,
                                                }};
        ImGui_ImplVulkan_Init(&imGuiInitInfo);
        ImGui_ImplVulkan_CreateFontsTexture();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        GetContext().WaitIdle();
        mContext->UnmapMemory(mUniformBuffer->GetAllocation());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        vkDestroyDescriptorPool(GetContext(), mImGuiDescriptorPool, nullptr);
    }

    void VulkanRenderer::BeginFrame()
    {
        gEngine.Context().PollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        GetResourceLoader().UpdateLights();
        GetResourceLoader().UploadModels();

        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        GetContext().WaitForFence(fence, 1000000000);
        GetContext().ResetFence(fence);
        VulkanUtils::HandleResult(GetSwapchain().AcquireNextImage(renderSemaphore),
                                  [&](VkResult)
                                  {
                                      GetContext().WaitIdle();
                                      GetSwapchain().RecreateSwapchain(gEngine.Context().GetWindowSize());
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
        command.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        ImGui::ShowDemoWindow();
        ImGui::Render();

        command.TransitionImageLayout(*mRenderImage, ImageLayout::eUndefined, ImageLayout::eColorAttachment);
        command.TransitionImageLayout(*mDepthImage, ImageLayout::eUndefined, ImageLayout::eDepthAttachment);

        command.BeginRendering(currentImage.GetView(), mDepthImage->GetView(), GetSwapchain().GetExtent());

        // RenderMesh(command);
        RenderGeometry(command);

        command.EndRendering();

        command.TransitionImageLayout(*mRenderImage, ImageLayout::eColorAttachment, ImageLayout::eTransferSrc);
        command.TransitionImageLayout(currentImage, ImageLayout::eUndefined, ImageLayout::eTransferDst);

        command.BlitImage(*mRenderImage, currentImage, VulkanUtils::GetImageBlit2());

        // command.BeginRendering(currentImage.GetView(), nullptr, GetSwapchain().GetExtent(), false);
        //
        // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
        //
        // command.EndRendering();

        command.TransitionImageLayout(currentImage, ImageLayout::eTransferDst, ImageLayout::ePresent);

        command.End();
    }

    void VulkanRenderer::EndFrame()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        VulkanUtils::HandleResult(GetSwapchain().Present(command, renderSemaphore, presentSemaphore, fence),
                                  [&](VkResult)
                                  {
                                      GetContext().WaitIdle();
                                      GetSwapchain().RecreateSwapchain(gEngine.Context().GetWindowSize());
                                      mRenderImage = std::make_unique<VulkanImage>(mContext,
                                                                                   ImageType::e2D,
                                                                                   VK_FORMAT_R16G16B16A16_SFLOAT,
                                                                                   GetSwapchain().GetExtent(),
                                                                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                                                   VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
                                      mDepthImage = std::make_unique<VulkanImage>(mContext,
                                                                                  ImageType::e2D,
                                                                                  VK_FORMAT_D32_SFLOAT,
                                                                                  GetSwapchain().GetExtent(),
                                                                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                                  VK_IMAGE_ASPECT_DEPTH_BIT);
                                  });
        mFrameIndex = (mFrameIndex + 1) % VulkanConstants::MaxFramesInFlight;
        ImGui::EndFrame();
    }

    void VulkanRenderer::RenderGeometry(const VulkanCommand& command)
    {
        command.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, GetGeometryPipeline());

        const auto& camera = gEngine.GetSystem<CameraSystem>();
        mUBO.mCamPos = camera.GetPosition();
        mUBO.mLightCount = static_cast<uint32_t>(gEngine.ECS().View<Component::Light>().size());
        mUBO.mProjection = camera.GetProjectionMatrix();
        mUBO.mView = camera.GetViewMatrix();
        memcpy(mappedBuffer, &mUBO, sizeof(UBO));

        for (auto [index, model] : std::views::enumerate(GetResourceLoader().GetModels()))
        {
            command.BindIndexBuffer(model.mIndexBuffer, 0);

            command.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      GetGeometryPipeline().GetLayout(),
                                      GetResourceLoader().GetDescriptor());

            auto parent = glm::mat4(1.f);

            for (const auto& nodeIndex : model.mRootNodes)
            {
                auto& node = model.mNodes[nodeIndex];
                RenderNodes(command, model, node, parent);
            }
        }
    }

    void VulkanRenderer::RenderNodes(const VulkanCommand& command,
                                     VulkanModel& model,
                                     Node& root,
                                     const glm::mat4& parentTransform)
    {
        auto& [mTransform, mMeshIndex, mLightIndex, mChildren] = root;

        glm::mat4 currentTransform = parentTransform;
        if (mMeshIndex != -1)
        {
            const auto& [mVertexOffset, mIndexOffset, mIndexCount, mMaterialIndex] = model.mMeshes[mMeshIndex];
            currentTransform = parentTransform * mTransform;
            const ModelPushConstant pushConstant{.mModel = currentTransform,
                                                 .mVertexAddress = model.mVertexBufferAddress,
                                                 .mMaterialAddress = model.mMaterialBufferAddress,
                                                 .mMaterialBaseIndex = mMaterialIndex};
            command.SetPushConstants(GetGeometryPipeline().GetLayout(),
                                     VK_SHADER_STAGE_ALL,
                                     sizeof(ModelPushConstant),
                                     &pushConstant);
            command.DrawIndexed(mIndexCount, 1, mIndexOffset, mVertexOffset);
        }
        for (const auto& childNodeIndex : mChildren)
        {
            auto& childNode = model.mNodes[childNodeIndex];
            RenderNodes(command, model, childNode, currentTransform);
        }
    }

    void VulkanRenderer::RenderMesh(const VulkanCommand& command) const
    {
        command.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, *mMeshPipeline);
        command.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  mMeshPipeline->GetLayout(),
                                  GetResourceLoader().GetDescriptor());
        command.DrawMeshEXT(1, 1, 1);
    }
}  // namespace FS