#pragma once
#include "Tools/Enums.hpp"

namespace FS
{
    class Window;
    class VulkanDescriptor;
    class VulkanFence;
    class VulkanSemaphore;
    class VulkanQueue;
    class VulkanCommand;
    class VulkanBuffer;
    class VulkanContext : public std::enable_shared_from_this<VulkanContext>
    {
    public:
        VulkanContext();
        ~VulkanContext();

        NON_MOVABLE(VulkanContext);
        NON_COPYABLE(VulkanContext);
        UNDERLYING(VkDevice, Device)

        [[nodiscard]] VkInstance GetInstance() const { return mInstance; }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
        [[nodiscard]] VmaAllocator GetAllocator() const { return mAllocator; }
        [[nodiscard]] VulkanQueue& GetGraphicsQueue() const { return *mGraphicsQueue; }
        [[nodiscard]] std::shared_ptr<VulkanQueue> GetSharedGraphicsQueue() const { return mGraphicsQueue; }
        [[nodiscard]] VulkanQueue& GetComputeQueue() const { return *mComputeQueue; }
        [[nodiscard]] std::shared_ptr<VulkanQueue> GetSharedComputeQueue() const { return mComputeQueue; }
        [[nodiscard]] VulkanQueue& GetTransferQueue() const { return *mTransferQueue; }
        [[nodiscard]] std::shared_ptr<VulkanQueue> GetSharedTransferQueue() const { return mTransferQueue; }
        [[nodiscard]] VkSurfaceKHR GetSurface() const { return mSurface; }

        [[nodiscard]] VulkanSemaphore CreateSemaphore(VkSemaphoreCreateFlags flags = {});
        [[nodiscard]] VulkanFence CreateFence(VkFenceCreateFlags flags = {});
        [[nodiscard]] VulkanCommand CreateCommand(const VulkanQueue& queue, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        [[nodiscard]] std::pair<VkImage, VmaAllocation> CreateImage(ImageType type,
                                                                    VkFormat format,
                                                                    VkExtent2D extent,
                                                                    VkImageUsageFlags usage) const;
        [[nodiscard]] VkImageView CreateImageView(VkImage image,
                                                  ImageType viewType,
                                                  VkFormat format,
                                                  VkImageAspectFlags aspectFlags) const;

        [[nodiscard]] std::tuple<VkBuffer, VmaAllocation, VmaAllocationInfo> CreateBuffer(BufferType bufferType,
                                                                                          uint32_t allocSize) const;
        [[nodiscard]] VmaAllocationInfo GetAllocationInfo(VmaAllocation allocation) const;

        [[nodiscard]] void* MapMemory(VmaAllocation allocation) const;
        void UnmapMemory(VmaAllocation allocation) const;

        [[nodiscard]] VkDescriptorPool CreateDescriptorPool(uint32_t maxSets, ArrayProxy<VkDescriptorPoolSize> poolSizes) const;
        [[nodiscard]] VkDescriptorSetLayout CreateDescriptorSetLayout(ArrayProxy<VkDescriptorSetLayoutBinding> bindings) const;
        [[nodiscard]] VkDescriptorSet AllocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) const;
        void UpdateDescriptorUniformBuffer(VkBuffer buffer, VkDescriptorSet set, uint32_t arrayIndex) const;
        void UpdateDescriptorStorageBuffer(VkBuffer buffer, VkDescriptorSet set, uint32_t arrayIndex) const;
        void UpdateDescriptorImage(VkSampler sampler, VkImageView view, VkDescriptorSet set, uint32_t arrayIndex) const;

        [[nodiscard]] VkShaderModule CreateShaderModule(const std::string& codePath) const;
        [[nodiscard]] VkPipeline CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const;
        [[nodiscard]] VkPipelineLayout CreatePipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const;

        void WaitForFence(VkFence fence, uint64_t timeout) const;
        void ResetFence(VkFence fence) const;

        void WaitIdle() const;

    private:
        void CreateInstance();
        void ChoosePhysicalDevice();
        void CreateDevice();
        void CreateQueues();
        void CreateAllocator();

        vkb::Instance mInstance;
        vkb::PhysicalDevice mPhysicalDevice;
        vkb::Device mDevice;
        VmaAllocator mAllocator{};
        
        std::shared_ptr<VulkanQueue> mGraphicsQueue;
        std::shared_ptr<VulkanQueue> mComputeQueue;
        std::shared_ptr<VulkanQueue> mTransferQueue;

        VkSurfaceKHR mSurface{};
        
    };
}  // namespace FS