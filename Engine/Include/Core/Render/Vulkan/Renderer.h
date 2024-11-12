#pragma once
#include "Core/Render/Renderer.h"
#include "Core/Render/Vulkan/Command.h"

namespace FS::VK
{
    class ModelManager;
    class Model;
    class GeometryPipeline;
    class Image;
    class Swapchain;
    class Device;
    class Queue;
    class Renderer final : public FS::Renderer
    {
    public:
        Renderer();
        ~Renderer() override;

        NON_COPYABLE(Renderer);
        NON_MOVABLE(Renderer);
        
        void BeginFrame() override;
        void Render() override;
        void EndFrame() override;

        [[nodiscard]] Device& GetDevice() const { return *mDevice; }
        [[nodiscard]] Swapchain& GetSwapchain() const { return *mSwapchain; }
        [[nodiscard]] Queue& GetGraphicsQueue() const { return *mGraphicsQueue; }
        [[nodiscard]] Queue& GetTransferQueue() const { return *mTransferQueue; }
        [[nodiscard]] ModelManager& GetModelManager() const { return *mModelManager; }
        [[nodiscard]] Image& GetRenderImage() const { return *mRenderImage; }
        [[nodiscard]] GeometryPipeline& GetGeometryPipeline() const { return *mGeometryPipeline; }

    private:
        void RenderGeometry() const;
        
        struct FrameData
        {
            Command mCommand;
            vk::raii::Semaphore mRenderSemaphore;
            vk::raii::Semaphore mPresentSemaphore;
            vk::raii::Fence mFence;

            FrameData(Command&& command, vk::raii::Semaphore&& renderSemaphore, vk::raii::Semaphore&& presentSemaphore,
                      vk::raii::Fence&& fence)
                : mCommand(std::move(command)), mRenderSemaphore(std::move(renderSemaphore)),
                  mPresentSemaphore(std::move(presentSemaphore)), mFence(std::move(fence))
            {
            }
        };

        [[nodiscard]] FrameData& GetCurrentFrame() const { return *mFrameData[mFrameIndex]; }
        std::array<std::unique_ptr<FrameData>, 3> mFrameData;
        uint32_t mFrameIndex = 0;

        std::unique_ptr<GeometryPipeline> mGeometryPipeline;
        std::unique_ptr<Model> mMesh;

        std::unique_ptr<Image> mRenderImage;
        vk::Extent2D mSwapchainExtent;

        std::unique_ptr<ModelManager> mModelManager;
        std::shared_ptr<Device> mDevice;
        std::shared_ptr<Swapchain> mSwapchain;
        std::shared_ptr<Queue> mGraphicsQueue;
        std::shared_ptr<Queue> mTransferQueue;
        bool swapchainResized = false;
    };
}
