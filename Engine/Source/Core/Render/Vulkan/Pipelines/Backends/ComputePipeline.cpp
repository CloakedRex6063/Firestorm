#include "Core/Render/Vulkan/Pipelines/Backend/ComputePipeline.h"
#include "Core/Render/Vulkan/Device.h"

namespace FS::VK
{
    ComputePipeline::ComputePipeline(const std::string& shaderPath, const Device& device)
    {
        CreateLayout(device);
        CreatePipeline(shaderPath, device);
    }
    void ComputePipeline::CreateLayout(const Device& device)
    {
        constexpr auto pipelineInfo = vk::PipelineLayoutCreateInfo()
                                          .setSetLayoutCount(0)
                                          .setPSetLayouts(nullptr)
                                          .setPushConstantRangeCount(0)
                                          .setPPushConstantRanges(nullptr);

        mLayout = device.CreatePipelineLayout(pipelineInfo);
    }
    void ComputePipeline::CreatePipeline(const std::string& shaderPath, const Device& device)
    {
        auto [vertexStage, vertexModule] = device.CreateShaderStage(vk::ShaderStageFlagBits::eCompute, shaderPath);
        const auto createInfo = vk::ComputePipelineCreateInfo().setLayout(GetLayout()).setStage(vertexStage);
        mPipeline = device.CreateComputePipeline(createInfo);
    }
} // namespace FS::VK