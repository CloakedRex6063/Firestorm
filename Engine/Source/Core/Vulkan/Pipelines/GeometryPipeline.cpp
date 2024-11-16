#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"

#include <Core/Render/Resources/Model.hpp>

#include "Core/Engine.h"
#include "Core/FileSystem.h"

namespace FS::VK
{
    GeometryPipeline::GeometryPipeline(const std::shared_ptr<Context>& context, const glm::uvec2& size)
        : mPipelineBuilder(context)
    {
        const auto vertexShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders, "triangle.vert.spv");
        const auto fragmentShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders, "triangle.frag.spv");
        mPipelineBuilder.AddVertexShader(vertexShader)
            .AddFragmentShader(fragmentShader)
            .SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetViewportAndScissor(size)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetCullMode(VK_CULL_MODE_BACK_BIT)
            .SetFrontFace(VK_FRONT_FACE_CLOCKWISE)
            .AddInputBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX)
            .AddInputAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mPosition))
            .AddInputAttribute(0, 1, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, mUVx))
            .AddInputAttribute(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mNormal))
            .AddInputAttribute(0, 3, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, mUVy))
            .AddInputAttribute(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, mColor))
            .Build();
    }
}  // namespace FS::VK