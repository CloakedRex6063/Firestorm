#include "Core/Render/Vulkan/Pipelines/VulkanMeshPipeline.h"
#include "Core/Engine.h"
#include "Core/FileIO.h"
#include "Core/Render/Vulkan/Resources/VulkanModel.h"

namespace FS
{
    VulkanMeshPipeline::VulkanMeshPipeline(const std::shared_ptr<VulkanContext>& context,
                                                   VkDescriptorSetLayout setLayout,
                                                   const glm::uvec2& size)
        : mPipelineBuilder(context)
    {
        const auto meshShader = gEngine.FileIO().GetPath(Directory::eEngineShaders, "mesh.glsl.spv");
        const auto fragmentShader = gEngine.FileIO().GetPath(Directory::eEngineShaders, "meshShading.frag.spv");

        VkPushConstantRange pushConstantRange = {.stageFlags = VK_SHADER_STAGE_ALL, .size = sizeof(ModelPushConstant)};

        mPipelineBuilder.AddMeshShader(meshShader)
            .AddFragmentShader(fragmentShader)
            .SetViewportAndScissor(size)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetCullMode(VK_CULL_MODE_BACK_BIT)
            .SetFrontFace(VK_FRONT_FACE_CLOCKWISE)
            .SetDescriptorLayouts(setLayout)
            .SetPushConstants(pushConstantRange)
            .EnableDepthTest()
            .EnableBlending()
            .Build();
    }
}  // namespace FS