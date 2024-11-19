#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Engine.h"
#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/Systems/FileSystem.h"

namespace FS::VK
{
    GeometryPipeline::GeometryPipeline(const std::shared_ptr<Context>& context,
                                       VkDescriptorSetLayout setLayout,
                                       const glm::uvec2& size)
        : mPipelineBuilder(context)
    {
        const auto vertexShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders, "mesh.vert.spv");
        const auto fragmentShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders, "mesh.frag.spv");

        VkPushConstantRange pushConstantRange = {.stageFlags = VK_SHADER_STAGE_ALL, .size = sizeof(ModelPushConstant)};

        mPipelineBuilder.AddVertexShader(vertexShader)
            .AddFragmentShader(fragmentShader)
            .SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetViewportAndScissor(size)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetCullMode(VK_CULL_MODE_BACK_BIT)
            .SetFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetDescriptorLayouts(setLayout)
            .SetPushConstants(pushConstantRange)
            .EnableDepthTest()
            .Build();
    }
}  // namespace FS::VK