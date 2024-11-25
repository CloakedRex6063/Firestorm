#include "Core/Render/Vulkan/Resources/VulkanDescriptor.h"
#include "Core/Render/Vulkan/VulkanConstants.hpp"
#include "Core/Render/Vulkan/VulkanContext.h"

namespace FS
{
    VulkanDescriptor::VulkanDescriptor(const std::shared_ptr<VulkanContext>& context) : mContext(context)
    {
        constexpr std::array poolSizes = {VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                               .descriptorCount = VulkanConstants::MaxSamplerDescriptors},
                                          VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                               .descriptorCount = VulkanConstants::MaxUniformDescriptors},
                                          VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                               .descriptorCount = VulkanConstants::MaxStorageDescriptors}};

        mPool = mContext->CreateDescriptorPool(1, poolSizes);

        constexpr std::array bindings = {
            VkDescriptorSetLayoutBinding{.binding = VulkanConstants::SamplerBinding,
                                         .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         .descriptorCount = VulkanConstants::MaxSamplerDescriptors,
                                         .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT},
            VkDescriptorSetLayoutBinding{.binding = VulkanConstants::UniformBinding,
                                         .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         .descriptorCount = VulkanConstants::MaxUniformDescriptors,
                                         .stageFlags = VK_SHADER_STAGE_ALL},
            VkDescriptorSetLayoutBinding{.binding = VulkanConstants::StorageBinding,
                                         .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         .descriptorCount = VulkanConstants::MaxStorageDescriptors,
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