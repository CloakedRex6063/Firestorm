#pragma once

namespace FS::VK
{
    class Descriptor
    {
    public:
        //Descriptor(const vk::raii::DescriptorPool& descriptorPool);
    private:
        vk::raii::DescriptorSet mDescriptorSet;
        vk::raii::DescriptorSetLayout mDescriptorSetLayout;
    };
}