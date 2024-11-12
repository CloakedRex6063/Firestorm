#pragma once

namespace FS::VK
{
    class Device;
    struct GraphicsPipelineBuilder;

    class ComputePipeline
    {
    public:
        ComputePipeline(const std::string& shaderPath, const Device& device);
        operator vk::raii::Pipeline&() const { return *mPipeline; }
        [[nodiscard]] vk::raii::PipelineLayout& GetLayout() const { return *mLayout; }

    private:
        void CreateLayout(const Device& device);
        void CreatePipeline(const std::string& shaderPath, const Device& device);
        std::unique_ptr<vk::raii::Pipeline> mPipeline;
        std::unique_ptr<vk::raii::PipelineLayout> mLayout;
    };
}