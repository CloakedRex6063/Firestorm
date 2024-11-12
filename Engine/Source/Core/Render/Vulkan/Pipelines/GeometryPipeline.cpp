#include "Core/Render/Vulkan/Pipelines/GeometryPipeline.h"
#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Render/Vulkan/Resources/Model.h"

namespace FS::VK
{
    GeometryPipeline::GeometryPipeline(const Device& device, const glm::uvec2& size)
    {
        GraphicsPipelineBuilder builder;
        
        constexpr auto pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex).setSize(sizeof(MeshPushConstants));
        const auto layoutCreateInfo = vk::PipelineLayoutCreateInfo().setPushConstantRanges(pushConstantRange);
        builder.mLayoutCreateInfo = layoutCreateInfo;
        
        builder.mWidth = size.x;
        builder.mHeight = size.y;
        builder.mCullMode = vk::CullModeFlagBits::eBack;
        builder.mPolygonMode = vk::PolygonMode::eFill;
        builder.mSetLayout = nullptr;

        const auto vertexShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders,"triangle.vert.spv");
        const auto fragmentShader = gEngine.FileSystem().GetPath(Directory::eEngineShaders,"triangle.frag.spv"); 
        builder.mShaderPaths = {vertexShader, fragmentShader};

        builder.mColorBlendAttachment =
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
        
        mPipeline = std::make_unique<GraphicsPipeline>(builder, device);
    }
} // namespace FS::VK