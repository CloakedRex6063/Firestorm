#pragma once 

namespace FS::VK
{
    class Context;
    class Descriptor 
    {
    public:
        Descriptor(const std::shared_ptr<Context>& context);
        ~Descriptor();
        NON_COPYABLE(Descriptor)
        NON_MOVABLE(Descriptor)
        UNDERLYING(VkDescriptorSet, Set);

        [[nodiscard]] VkDescriptorSet GetSet() const { return mSet; }
        [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return mLayout; }
        [[nodiscard]] VkDescriptorPool GetPool() const { return mPool; }

    private:
        std::shared_ptr<Context> mContext;

        VkDescriptorPool mPool;
        VkDescriptorSetLayout mLayout;
        VkDescriptorSet mSet;
    };
}