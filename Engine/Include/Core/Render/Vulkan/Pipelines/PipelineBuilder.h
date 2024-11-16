#pragma once

namespace FS::VK
{
    class Context;
    class PipelineBuilder
    {
    public:
        PipelineBuilder(const std::shared_ptr<Context>& context);
        ~PipelineBuilder();

        UNDERLYING(VkPipeline, Pipeline)
        [[nodiscard]] VkPipelineLayout GetLayout() const { return mPipelineLayout; }

#pragma region Mandatory
        PipelineBuilder& AddVertexShader(const std::string& codePath);
        PipelineBuilder& AddFragmentShader(const std::string& codePath);
        PipelineBuilder& SetTopology(VkPrimitiveTopology topology);
        PipelineBuilder& SetViewportAndScissor(const glm::uvec2& size);
        PipelineBuilder& SetPolygonMode(VkPolygonMode polygonMode);
        PipelineBuilder& SetCullMode(VkCullModeFlagBits cullMode);
        PipelineBuilder& SetFrontFace(VkFrontFace frontFace);
#pragma endregion

#pragma region Optional
        template <typename PushConstants>
        PipelineBuilder& SetPushConstants(ArrayProxy<PushConstants> constants);
        PipelineBuilder& SetDescriptorLayouts(ArrayProxy<VkDescriptorSetLayout> setLayouts);

        PipelineBuilder& AddInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
        PipelineBuilder& AddInputAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
        
        PipelineBuilder& EnableMSAA(VkSampleCountFlagBits sampleCount);
        PipelineBuilder& EnableBlending();
#pragma endregion

        void Build();

    private:
        std::shared_ptr<Context> mContext;
        
        std::array<VkPipelineShaderStageCreateInfo, 2> mShaderStages = {};
        std::array<VkShaderModule, 2> mShaderModules = {};
        VkViewport mViewport = {};
        VkRect2D mScissor = {};
        VkPipelineDynamicStateCreateInfo mDynamicState{};
        VkPipelineVertexInputStateCreateInfo mVertexInputState{};
        std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions = {};
        std::vector<VkVertexInputAttributeDescription> mVertexAttributeDescriptions = {};
        VkPipelineInputAssemblyStateCreateInfo mInputAssembly{};
        VkPipelineViewportStateCreateInfo mViewportState{};
        VkPipelineRasterizationStateCreateInfo mRasterizationState{};
        VkPipelineMultisampleStateCreateInfo mMsaaState{};
        VkPipelineColorBlendAttachmentState mColorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo mColorBlendState{};
        VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};

        VkPipeline mPipeline = nullptr;
        VkPipelineLayout mPipelineLayout = nullptr;
    };

    template <typename PushConstants>
    PipelineBuilder& PipelineBuilder::SetPushConstants(ArrayProxy<PushConstants> constants)
    {
        mPipelineLayoutInfo.pushConstantRangeCount = constants.size();
        mPipelineLayoutInfo.pPushConstantRanges = constants.data();
        return *this;
    }
}  // namespace FS::VK
