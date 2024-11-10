#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Engine.h"
#include "Core/FileSystem.h"


namespace FS::VK
{
    GeometryPipeline::GeometryPipeline(const Device& device, const glm::uvec2& size)
    {
        GraphicsPipelineBuilder builder;
        builder.mWidth = size.x;
        builder.mHeight = size.y;
        builder.mCullMode = vk::CullModeFlagBits::eNone;
        builder.mPolygonMode = vk::PolygonMode::eFill;
        builder.mSetLayout = nullptr;

        const auto vertexShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders,"triangle.vert.spv");
        const auto fragmentShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders,"triangle.frag.spv"); 
        builder.mShaderPaths = {vertexShader, fragmentShader};

        constexpr auto colorBlendAttachment =
            vk::PipelineColorBlendAttachmentState()
                .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                .setBlendEnable(true)
                .setColorBlendOp(vk::BlendOp::eAdd)
                .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                .setDstColorBlendFactor(vk::BlendFactor::eZero)
                .setAlphaBlendOp(vk::BlendOp::eAdd)
                .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                .setDstAlphaBlendFactor(vk::BlendFactor::eZero);
        builder.mColorBlendAttachment = colorBlendAttachment;
        
        mPipeline = std::make_unique<GraphicsPipeline>(builder, device);
    }
} // namespace FS::VK