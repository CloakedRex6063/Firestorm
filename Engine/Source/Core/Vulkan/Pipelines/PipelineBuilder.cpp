#include "Core/Render/Vulkan/Pipelines/PipelineBuilder.h"
#include "Core/Render/Vulkan/Context.h"
#include "Core/Render/Vulkan/Resources/Model.h"
#include "Core/Render/Vulkan/Tools/Utils.h"

namespace FS::VK
{
    PipelineBuilder::PipelineBuilder(const std::shared_ptr<Context>& context) : mContext(context)
    {
        mPipelineLayoutInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

        mDynamicState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};

        mVertexInputState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        mInputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                          .primitiveRestartEnable = false};

        mViewportState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                          .viewportCount = 1,
                          .scissorCount = 1};

        mRasterizationState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .depthBiasEnable = false,
            .depthBiasConstantFactor = 0.f,

            .depthBiasClamp = false,
            .depthBiasSlopeFactor = 0.f,
            .lineWidth = 1.f,
        };

        mMsaaState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                      .sampleShadingEnable = false,
                      .minSampleShading = 1.f,
                      .alphaToCoverageEnable = false,
                      .alphaToOneEnable = false};

        mColorBlendAttachment = {
            .blendEnable = false,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        mColorBlendState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                            .logicOpEnable = false,
                            .logicOp = VK_LOGIC_OP_COPY,
                            .attachmentCount = 1,
                            .pAttachments = &mColorBlendAttachment};

        mDepthStencilState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    }

    PipelineBuilder::~PipelineBuilder()
    {
        vkDestroyPipelineLayout(*mContext, mPipelineLayout, nullptr);
        vkDestroyPipeline(*mContext, mPipeline, nullptr);
    }

#pragma region Mandatory

    PipelineBuilder& PipelineBuilder::SetDescriptorLayouts(const ArrayProxy<VkDescriptorSetLayout> setLayouts)
    {
        mPipelineLayoutInfo.setLayoutCount = setLayouts.size();
        mPipelineLayoutInfo.pSetLayouts = setLayouts.data();
        return *this;
    }

    PipelineBuilder& PipelineBuilder::AddInputBinding(const uint32_t binding,
                                                      const uint32_t stride,
                                                      const VkVertexInputRate inputRate)
    {
        VkVertexInputBindingDescription bindingDescription = {
            .binding = binding,
            .stride = stride,
            .inputRate = inputRate,
        };
        mVertexInputBindingDescriptions.emplace_back(bindingDescription);
        return *this;
    }

    PipelineBuilder& PipelineBuilder::AddInputAttribute(const uint32_t binding,
                                                        const uint32_t location,
                                                        const VkFormat format,
                                                        const uint32_t offset)
    {
        VkVertexInputAttributeDescription attributeDescription = {.location = location,
                                                                  .binding = binding,
                                                                  .format = format,
                                                                  .offset = offset};
        mVertexAttributeDescriptions.emplace_back(attributeDescription);
        return *this;
    }

    PipelineBuilder& PipelineBuilder::AddVertexShader(const std::string& codePath)
    {
        mShaderModules[0] = mContext->CreateShaderModule(codePath);
        mShaderStages[0] = Utils::CreateShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, mShaderModules[0]);
        return *this;
    }

    PipelineBuilder& PipelineBuilder::AddFragmentShader(const std::string& codePath)
    {
        mShaderModules[1] = mContext->CreateShaderModule(codePath);
        mShaderStages[1] = Utils::CreateShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, mShaderModules[1]);
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetTopology(const VkPrimitiveTopology topology)
    {
        mInputAssembly.topology = topology;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetViewportAndScissor(const glm::uvec2& size)
    {
        mViewport = {
            .width = static_cast<float>(size.x),
            .height = static_cast<float>(size.y),
            .minDepth = 1.f,
            .maxDepth = 0.f,
        };

        mScissor = {.extent = {size.x, size.y}};

        mViewportState.pScissors = &mScissor;
        mViewportState.pViewports = &mViewport;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetPolygonMode(const VkPolygonMode polygonMode)
    {
        mRasterizationState.polygonMode = polygonMode;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetCullMode(const VkCullModeFlagBits cullMode)
    {
        mRasterizationState.cullMode = cullMode;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetFrontFace(const VkFrontFace frontFace)
    {
        mRasterizationState.frontFace = frontFace;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetPushConstants(const ArrayProxy<VkPushConstantRange> constants)
    {
        mPipelineLayoutInfo.pushConstantRangeCount = constants.size();
        mPipelineLayoutInfo.pPushConstantRanges = constants.data();
        return *this;
    }

#pragma endregion

#pragma region Optional

    PipelineBuilder& PipelineBuilder::EnableMSAA(const VkSampleCountFlagBits sampleCount)
    {
        mMsaaState.rasterizationSamples = sampleCount;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::EnableBlending()
    {
        mColorBlendAttachment.blendEnable = true;
        mColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        mColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        mColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        mColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        mColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        mColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::EnableDepthTest()
    {
        mDepthStencilState.depthTestEnable = true;
        mDepthStencilState.depthWriteEnable = true;
        mDepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        mDepthStencilState.depthBoundsTestEnable = false;
        mDepthStencilState.stencilTestEnable = false;
        mDepthStencilState.minDepthBounds = 0.f;
        mDepthStencilState.maxDepthBounds = 1.f;
        return *this;
    }

#pragma endregion

    void PipelineBuilder::Build()
    {
        mPipelineLayout = mContext->CreatePipelineLayout(mPipelineLayoutInfo);

        constexpr std::array dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        mDynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        mDynamicState.pDynamicStates = dynamicStates.data();

        // change to VK_FORMAT_R16G16B16A16_SFLOAT when using a render image
        constexpr auto format = VK_FORMAT_B8G8R8A8_UNORM;
        VkPipelineRenderingCreateInfo renderInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                                                 .colorAttachmentCount = 1,
                                                 .pColorAttachmentFormats = &format,
                                                 .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT};

        const VkGraphicsPipelineCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                                         .pNext = &renderInfo,
                                                         .stageCount = static_cast<uint32_t>(mShaderStages.size()),
                                                         .pStages = mShaderStages.data(),
                                                         .pVertexInputState = &mVertexInputState,
                                                         .pInputAssemblyState = &mInputAssembly,
                                                         .pViewportState = &mViewportState,
                                                         .pRasterizationState = &mRasterizationState,
                                                         .pMultisampleState = &mMsaaState,
                                                         .pDepthStencilState = &mDepthStencilState,
                                                         .pColorBlendState = &mColorBlendState,
                                                         .pDynamicState = &mDynamicState,
                                                         .layout = mPipelineLayout};
        mPipeline = mContext->CreateGraphicsPipeline(createInfo);

        for (const auto& shader : mShaderModules)
        {
            vkDestroyShaderModule(*mContext, shader, nullptr);
        }
    }

}  // namespace FS::VK