#pragma once
#include "Core/Render/Vulkan/VulkanConstants.hpp"
#include "VulkanCommand.h"
#include "VulkanSync.h"
#include "ktxvulkan.h"
#include "Core/Render/Renderer.h"
#include "Resources/VulkanModel.h"

namespace FS
{
    class VulkanMeshPipeline;
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
        [[nodiscard]] VulkanResourceLoader& GetResourceLoader() const { return *mResourceLoader; }
        [[nodiscard]] VulkanGeometryPipeline& GetGeometryPipeline() const { return *mGeometryPipeline; }

    private:
        struct FrameData;
        [[nodiscard]] FrameData& GetFrameData() const { return *mFrameData[mFrameIndex]; }

        void RenderGeometry(const VulkanCommand& command);
        void RenderNodes(const VulkanCommand& command, VulkanModel& model, Node& root, const glm::mat4& parentTransform);
        void RenderMesh(const VulkanCommand& command) const;

        std::shared_ptr<VulkanContext> mContext;
        std::unique_ptr<VulkanSwapchain> mSwapchain;
        std::unique_ptr<VulkanGeometryPipeline> mGeometryPipeline;
        std::unique_ptr<VulkanMeshPipeline> mMeshPipeline;

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
        struct UBO
        {
            glm::vec3 mCamPos;
            uint32_t mLightCount;
            glm::mat4 mView;
            glm::mat4 mProjection;
        } mUBO{};
        void* mappedBuffer;

        VkDescriptorPool mImGuiDescriptorPool;

        std::unique_ptr<VulkanImage> mDepthImage;
        std::unique_ptr<VulkanImage> mRenderImage;
        std::unique_ptr<VulkanResourceLoader> mResourceLoader;
    };
}  // namespace FS