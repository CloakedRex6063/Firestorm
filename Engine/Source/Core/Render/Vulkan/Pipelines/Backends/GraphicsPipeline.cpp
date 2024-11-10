#include "Core/Render/Vulkan/Pipelines/Backend/GraphicsPipeline.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineBuilder& builder, const Device& device)
    {
        CreateLayout(device);
        CreatePipeline(builder, device);
    }

    void GraphicsPipeline::CreateLayout(const Device& device)
    {
        constexpr auto pipelineInfo = vk::PipelineLayoutCreateInfo()
                                      .setSetLayoutCount(0)
                                      .setPSetLayouts(nullptr)
                                      .setPushConstantRangeCount(0)
                                      .setPPushConstantRanges(nullptr);

        mLayout = device.CreatePipelineLayout(pipelineInfo);
    }

    void GraphicsPipeline::CreatePipeline(const GraphicsPipelineBuilder& builder, const Device& device)
    {
        std::vector dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};
        auto dynamicCreateInfo = vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamicStates);

        auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
                                   .setVertexBindingDescriptions(builder.mVertexBindings)
                                   .setVertexAttributeDescriptions(builder.mVertexAttributes);

        auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
                                 .setTopology(vk::PrimitiveTopology::eTriangleList)
                                 .setPrimitiveRestartEnable(false);

        auto viewport = vk::Viewport()
                            .setWidth(static_cast<float>(builder.mWidth))
                            .setHeight(static_cast<float>(builder.mHeight))
                            .setMinDepth(0.0f)
                            .setMaxDepth(1.f);

        auto scissor = vk::Rect2D().setExtent(vk::Extent2D(builder.mWidth, builder.mHeight));

        auto viewportCreateInfo = vk::PipelineViewportStateCreateInfo().setViewports(viewport).setScissors(scissor);

        auto rasterization = vk::PipelineRasterizationStateCreateInfo()
                                 .setDepthClampEnable(false)
                                 .setFrontFace(vk::FrontFace::eClockwise)
                                 .setCullMode(builder.mCullMode)
                                 .setRasterizerDiscardEnable(false)
                                 .setPolygonMode(builder.mPolygonMode)
                                 .setLineWidth(1);

        auto msaa = vk::PipelineMultisampleStateCreateInfo().setMinSampleShading(1.f);
        auto depthStencil = vk::PipelineDepthStencilStateCreateInfo();

        auto colorBlending = vk::PipelineColorBlendStateCreateInfo()
                                 .setLogicOpEnable(false)
                                 .setLogicOp(vk::LogicOp::eCopy)
                                 .setAttachments(builder.mColorBlendAttachment);

        auto format = vk::Format::eR16G16B16A16Sfloat;
        auto renderingInfo = vk::PipelineRenderingCreateInfo().setColorAttachmentFormats(format);

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaders;
        auto [vertexStage, vertexModule] = device.CreateShaderStage(vk::ShaderStageFlagBits::eVertex, builder.mShaderPaths[0]);
        auto [fragmentStage, fragmentModule] = device.CreateShaderStage(vk::ShaderStageFlagBits::eFragment, builder.mShaderPaths[1]);
        shaders[0] = vertexStage;
        shaders[1] = fragmentStage;
        
        auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
                                      .setPNext(&renderingInfo)
                                      .setStages(shaders)
                                      .setPVertexInputState(&vertexInputInfo)
                                      .setPInputAssemblyState(&inputAssembly)
                                      .setPViewportState(&viewportCreateInfo)
                                      .setPRasterizationState(&rasterization)
                                      .setPMultisampleState(&msaa)
                                      .setPDepthStencilState(&depthStencil)
                                      .setPColorBlendState(&colorBlending)
                                      .setPDynamicState(&dynamicCreateInfo)
                                      .setLayout(*mLayout);
        mPipeline = device.CreateGraphicsPipeline(pipelineCreateInfo);
    }
} // namespace FS::VK
