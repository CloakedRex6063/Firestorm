#pragma once
#include "Core/Render/Vulkan/VulkanConstants.hpp"
#include "VulkanCommand.h"
#include "VulkanSync.h"
#include "Resources/VulkanModel.h"
#include "Core/Render/Renderer.h"

namespace FS
{
    struct Vertex;
    class VulkanResourceLoader;
    class VulkanImage;
    class VulkanSwapchain;
    class VulkanContext;
    class VulkanBuffer;
    class VulkanDescriptor;
    class VulkanGeometryPipeline;
    class VulkanRenderer final : public Renderer
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer() override;

        void BeginFrame() override;
        void Render() override;
        void EndFrame() override;

        [[nodiscard]] VulkanContext& GetContext() const { return *mContext; }
        [[nodiscard]] VulkanSwapchain& GetSwapchain() const { return *mSwapchain; }
        [[nodiscard]] VulkanResourceLoader& GetResourceLoader() const { return *mModelManager; }
        [[nodiscard]] VulkanGeometryPipeline& GetGeometryPipeline() const { return *mGeometryPipeline; }

    private:
        struct FrameData;
        [[nodiscard]] FrameData& GetFrameData() const { return *mFrameData[mFrameIndex]; }
        
        void RenderGeometry(const VulkanCommand& command) const;

        std::shared_ptr<VulkanContext> mContext;
        std::unique_ptr<VulkanSwapchain> mSwapchain;
        std::unique_ptr<VulkanGeometryPipeline> mGeometryPipeline;

        struct FrameData
        {
            VulkanFence mFence;
            VulkanSemaphore mRenderSemaphore;
            VulkanSemaphore mPresentSemaphore;
            VulkanCommand mCommand;
        };
        std::array<std::unique_ptr<FrameData>, VulkanConstants::MaxFramesInFlight> mFrameData;
        uint32_t mFrameIndex = 0;

        std::unique_ptr<VulkanBuffer> mUniformBuffer;
        void* mappedBuffer;
        VkDescriptorPool mImGuiDescriptorPool;

        std::unique_ptr<VulkanImage> mDepthImage;
        std::unique_ptr<VulkanResourceLoader> mModelManager;
    };
}  // namespace FS