#pragma once
#include "Core/Render/Vulkan/Constants.hpp"
#include "Command.h"
#include "Sync.h"
#include "Resources/Model.h"

namespace FS
{
    struct Vertex;
}

namespace FS::VK
{
    class ModelManager;
    class Image;
    class Swapchain;
    class Context;
    class Buffer;
    class Descriptor;
    class GeometryPipeline;
    class Renderer final : public FS::Renderer
    {
    public:
        Renderer();
        ~Renderer() override;

        void BeginFrame() override;
        void Render() override;
        void EndFrame() override;

        [[nodiscard]] Context& GetContext() const { return *mContext; }
        [[nodiscard]] Swapchain& GetSwapchain() const { return *mSwapchain; }
        [[nodiscard]] ModelManager& GetModelManager() const { return *mModelManager; }
        [[nodiscard]] GeometryPipeline& GetGeometryPipeline() const { return *mGeometryPipeline; }

    private:
        struct FrameData;
        [[nodiscard]] FrameData& GetFrameData() const { return *mFrameData[mFrameIndex]; }

        void RenderGeometry(const Command& command) const;

        std::shared_ptr<Context> mContext;
        std::unique_ptr<Swapchain> mSwapchain;
        std::unique_ptr<GeometryPipeline> mGeometryPipeline;

        struct FrameData
        {
            Fence mFence;
            Semaphore mRenderSemaphore;
            Semaphore mPresentSemaphore;
            Command mCommand;
        };
        std::array<std::unique_ptr<FrameData>, Constants::MaxFramesInFlight> mFrameData;
        uint32_t mFrameIndex = 0;

        std::unique_ptr<Image> mDepthImage;
        std::unique_ptr<ModelManager> mModelManager;
    };
}  // namespace FS::VK