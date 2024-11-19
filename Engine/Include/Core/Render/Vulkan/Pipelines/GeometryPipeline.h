#pragma once
#include "PipelineBuilder.h"

namespace FS::VK
{
    class GeometryPipeline
    {
    public:
        GeometryPipeline(const std::shared_ptr<Context>& context, VkDescriptorSetLayout setLayout, const glm::uvec2& size);

        VK_PIPELINE()

    private:
        PipelineBuilder mPipelineBuilder;
    };
}  // namespace FS::VK
