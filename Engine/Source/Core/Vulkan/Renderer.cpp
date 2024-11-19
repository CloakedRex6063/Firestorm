#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Renderer.h"
#include "Core/Engine.h"
#include "Core/Render/Resources/Model.hpp"
#include "Core/Render/Vulkan/Context.h"
#include "Core/Render/Vulkan/Queue.h"
#include "Core/Render/Vulkan/Swapchain.h"
#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Render/Vulkan/Resources/Buffer.h"
#include "Core/Render/Vulkan/Resources/Descriptor.h"
#include "Core/Render/Vulkan/Resources/Image.h"
#include "Core/Render/Vulkan/Tools/Utils.h"
#include "Core/Systems/FileSystem.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/Render/Vulkan/Resources/ModelManager.h"

namespace FS::VK
{
    Renderer::Renderer()
    {
        auto size = GetWindow().GetSize();

        mContext = std::make_shared<Context>(GetWindow());
        mSwapchain = std::make_unique<Swapchain>(mContext, size);
        mModelManager = std::make_unique<ModelManager>(mContext);
        mGeometryPipeline = std::make_unique<GeometryPipeline>(mContext, GetModelManager().GetDescriptor().GetLayout(), size);

        for (auto& frame : mFrameData)
        {
            frame = std::make_unique<FrameData>(GetContext().CreateFence(VK_FENCE_CREATE_SIGNALED_BIT),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateSemaphore(),
                                                GetContext().CreateCommand(GetContext().GetGraphicsQueue()));
        }

        mDepthImage = std::make_unique<Image>(mContext,
                                              ImageType::e2D,
                                              VK_FORMAT_D32_SFLOAT,
                                              GetSwapchain().GetExtent(),
                                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                              VK_IMAGE_ASPECT_DEPTH_BIT);

        const auto otherModelPath = FS::gEngine.FileSystem().GetPath(FS::Directory::eGameAssets, "Models/DamagedHelmet.glb");
        const auto modelPath = FS::gEngine.FileSystem().GetPath(FS::Directory::eGameAssets, "Models/chess/ABeautifulGame.gltf");
        auto model = FS::gEngine.ResourceSystem().LoadModel(modelPath).value();
        //auto otherModel = FS::gEngine.ResourceSystem().LoadModel(otherModelPath).value();
        auto modelsToUpload = gEngine.ResourceSystem().GetModelsToUpload();
        mModelManager->UploadModels(modelsToUpload);
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
                                GetContext().WaitIdle();
                                GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                                mDepthImage = std::make_unique<Image>(mContext,
                                                                      ImageType::e2D,
                                                                      VK_FORMAT_D32_SFLOAT,
                                                                      GetSwapchain().GetExtent(),
                                                                      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                      VK_IMAGE_ASPECT_DEPTH_BIT);
                            });
    }

    void Renderer::Render()
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

    void Renderer::EndFrame()
    {
        auto& [fence, renderSemaphore, presentSemaphore, command] = GetFrameData();
        Utils::HandleResult(GetSwapchain().Present(command, renderSemaphore, presentSemaphore, fence),
                            [&](VkResult)
                            {
                                GetContext().WaitIdle();
                                GetSwapchain().RecreateSwapchain(GetWindow().GetSize());
                                mDepthImage = std::make_unique<Image>(mContext,
                                                                      ImageType::e2D,
                                                                      VK_FORMAT_D32_SFLOAT,
                                                                      GetSwapchain().GetExtent(),
                                                                      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                      VK_IMAGE_ASPECT_DEPTH_BIT);
                            });
        mFrameIndex = (mFrameIndex + 1) % Constants::MaxFramesInFlight;
    }

    static float rotationAngle = 0.0f;
    void Renderer::RenderGeometry(const Command& command) const
    {
        command.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, GetGeometryPipeline());

        const auto size = GetWindow().GetSize();
        const auto aspect = static_cast<float>(size.x) / static_cast<float>(size.y);

        rotationAngle += 0.1f * gEngine.GetDeltaTime();
        constexpr float cameraRadius = 1.0f;

        const auto cameraPosition = glm::vec3(cameraRadius * cos(rotationAngle),  // X-coordinate
                                             0.0f,                               // Y-coordinate (fixed height)
                                             cameraRadius * sin(rotationAngle)   // Z-coordinate
        );

        glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f);

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

            VkBufferDeviceAddressInfo vertexBufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                                                       .buffer = model.GetVertexBuffer()};

            auto parentTransform = glm::mat4(1.0f);
            for (const auto& nodeIndex : model.GetRootNodeIndices())
            {
                auto& node = model.GetNodes()[nodeIndex];
                const auto& mesh = model.GetMeshes()[node.mMeshIndex];
                const auto textureIndex = model.GetMaterials()[mesh.mMaterialIndex].mBaseTextureIndex;
                ModelPushConstant pushConstant{.mMVP = projection * view * (parentTransform * node.mTransform),
                                               .mVertexAddress = vkGetBufferDeviceAddress(*mContext, &vertexBufferInfo),
                                               .mBaseTextureIndex = textureIndex};
                command.SetPushConstants(GetGeometryPipeline().GetLayout(),
                                         VK_SHADER_STAGE_ALL,
                                         sizeof(ModelPushConstant),
                                         &pushConstant);
                command.DrawIndexed(mesh.mIndexCount, 1, mesh.mIndexOffset, mesh.mVertexOffset);
                for (const auto& childNodeIndex : node.mChildren)
                {
                    auto& childNode = model.GetNodes()[childNodeIndex];
                    const auto& childMesh = model.GetMeshes()[childNode.mMeshIndex];
                    const auto childTextureIndex = model.GetMaterials()[childMesh.mMaterialIndex].mBaseTextureIndex;
                    pushConstant.mMVP = projection * view * (parentTransform * node.mTransform * childNode.mTransform);
                    pushConstant.mBaseTextureIndex = childTextureIndex;
                    command.SetPushConstants(GetGeometryPipeline().GetLayout(),
                                             VK_SHADER_STAGE_ALL,
                                             sizeof(ModelPushConstant),
                                             &pushConstant);
                    command.DrawIndexed(childMesh.mIndexCount, 1, childMesh.mIndexOffset, childMesh.mVertexOffset);
                }
            }
        }
    }
}  // namespace FS::VK