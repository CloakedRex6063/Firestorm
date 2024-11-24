#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/Constants.hpp"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanDescriptor::VulkanDescriptor(const std::shared_ptr<VulkanContext>& context) : mContext(context)
    {
        constexpr std::array poolSizes = {VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                               .descriptorCount = Constants::MaxSamplerDescriptors},
                                          VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                               .descriptorCount = Constants::MaxUniformDescriptors}};

        mPool = mContext->CreateDescriptorPool(1, poolSizes);

        constexpr std::array bindings = {
            VkDescriptorSetLayoutBinding{.binding = Constants::SamplerBinding,
                                         .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         .descriptorCount = Constants::MaxSamplerDescriptors,
                                         .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT},
            VkDescriptorSetLayoutBinding{.binding = Constants::UniformBinding,
                                         .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         .descriptorCount = Constants::MaxUniformDescriptors,
                                         .stageFlags = VK_SHADER_STAGE_ALL}};

        mLayout = mContext->CreateDescriptorSetLayout(bindings);

        mSet = mContext->AllocateDescriptorSet(mPool, mLayout);
    }

    VulkanDescriptor::~VulkanDescriptor()
    {
        vkDestroyDescriptorSetLayout(*mContext, mLayout, nullptr);
        vkDestroyDescriptorPool(*mContext, mPool, nullptr);
    };
}  // namespace FS