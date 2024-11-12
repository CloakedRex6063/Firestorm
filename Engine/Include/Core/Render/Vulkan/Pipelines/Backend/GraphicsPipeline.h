#pragma once

namespace FS::VK
{
    struct GraphicsPipelineBuilder
    {
        vk::PipelineLayoutCreateInfo mLayoutCreateInfo;
        uint32_t mHeight;
        uint32_t mWidth;
        vk::CullModeFlags mCullMode;
        vk::PolygonMode mPolygonMode;
        std::vector<vk::VertexInputBindingDescription> mVertexBindings;
        std::vector<vk::VertexInputAttributeDescription> mVertexAttributes;
        vk::PipelineColorBlendAttachmentState mColorBlendAttachment;
        vk::DescriptorSetLayout mSetLayout;
        std::vector<vk::PushConstantRange> mPushConstants;
        std::array<std::string, 2> mShaderPaths;
    };

    class Device;

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(const GraphicsPipelineBuilder& builder, const Device& device);

        operator vk::raii::Pipeline&() const { return *mPipeline; }
        [[nodiscard]] vk::raii::PipelineLayout& GetLayout() const { return *mLayout; }

    private:
        void CreatePipeline(const GraphicsPipelineBuilder& builder, const Device& device);
        std::unique_ptr<vk::raii::Pipeline> mPipeline;
        std::unique_ptr<vk::raii::PipelineLayout> mLayout;
    };
}