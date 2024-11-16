#pragma once

#include "Command.h"
#include "Sync.h"

namespace FS
{
    struct Vertex;
}

namespace FS::VK
{
    class Swapchain;
    class Context;
    class Buffer;
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
        std::array<std::unique_ptr<FrameData>, 3> mFrameData;
        uint32_t mFrameIndex = 0;
        
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        std::unique_ptr<Buffer> mVertexBuffer;
        std::unique_ptr<Buffer> mIndexBuffer;
    };
}  // namespace FS::VK