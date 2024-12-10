#pragma once

namespace FS
{
    class VulkanContext;
    class VulkanPipelineBuilder
    {
    public:
        VulkanPipelineBuilder(const std::shared_ptr<VulkanContext>& context);
        ~VulkanPipelineBuilder();

        UNDERLYING(VkPipeline, Pipeline)
        [[nodiscard]] VkPipelineLayout GetLayout() const { return mPipelineLayout; }

#pragma region Mandatory
        VulkanPipelineBuilder& AddMeshShader(const std::string& codePath);
        VulkanPipelineBuilder& AddVertexShader(const std::string& codePath);
        VulkanPipelineBuilder& AddFragmentShader(const std::string& codePath);
        VulkanPipelineBuilder& SetTopology(VkPrimitiveTopology topology);
        VulkanPipelineBuilder& SetViewportAndScissor(const glm::uvec2& size);
        VulkanPipelineBuilder& SetPolygonMode(VkPolygonMode polygonMode);
        VulkanPipelineBuilder& SetCullMode(VkCullModeFlagBits cullMode);
        VulkanPipelineBuilder& SetFrontFace(VkFrontFace frontFace);
#pragma endregion

#pragma region Optional
        VulkanPipelineBuilder& SetPushConstants(ArrayProxy<VkPushConstantRange> constants);
        VulkanPipelineBuilder& SetDescriptorLayouts(ArrayProxy<VkDescriptorSetLayout> setLayouts);

        VulkanPipelineBuilder& AddInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
        VulkanPipelineBuilder& AddInputAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
        
        VulkanPipelineBuilder& EnableMSAA(VkSampleCountFlagBits sampleCount);
        VulkanPipelineBuilder& EnableBlending();
        VulkanPipelineBuilder& EnableDepthTest();
#pragma endregion

        void Build();

    private:
        std::shared_ptr<VulkanContext> mContext;
        
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
        VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
        VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};

        VkPipeline mPipeline = nullptr;
        VkPipelineLayout mPipelineLayout = nullptr;
    };
}  // namespace FS
