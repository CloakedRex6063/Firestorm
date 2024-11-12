#pragma once
#include "Core/Render/Vulkan/Pipelines/Backend/GraphicsPipeline.h"

namespace FS::VK
{
    class Device;
    class GeometryPipeline
    {
    public:
        GeometryPipeline(const Device& device, const glm::uvec2& size);
        
        operator vk::raii::Pipeline&() const { return *mPipeline; }
        [[nodiscard]] vk::PipelineLayout GetLayout() const { return *mPipeline->GetLayout(); }
        
    private:
        std::unique_ptr<GraphicsPipeline> mPipeline;
    };
} // namespace FS::VK