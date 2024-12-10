#pragma once
#include "VulkanPipelineBuilder.h"

namespace FS
{
    class VulkanMeshPipeline
    {
    public:
        VulkanMeshPipeline(const std::shared_ptr<VulkanContext>& context, VkDescriptorSetLayout setLayout, const glm::uvec2& size);

        VK_PIPELINE()

    private:
        VulkanPipelineBuilder mPipelineBuilder;
    };
}  // namespace FS
