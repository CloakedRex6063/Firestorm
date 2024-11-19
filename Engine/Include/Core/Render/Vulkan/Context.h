#pragma once
#include "Tools/Enums.hpp"

namespace FS
{
    class Window;
}

namespace FS::VK
{
    class Descriptor;
    class Fence;
    class Semaphore;
    class Queue;
    class Command;
    class Buffer;
    class Context : public std::enable_shared_from_this<Context>
    {
    public:
        Context(const Window& window);
        ~Context();

        NON_MOVABLE(Context);
        NON_COPYABLE(Context);
        UNDERLYING(VkDevice, Device)

        [[nodiscard]] VkInstance GetInstance() const { return mInstance; }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
        [[nodiscard]] VmaAllocator GetAllocator() const { return mAllocator; }
        [[nodiscard]] Queue& GetGraphicsQueue() const { return *mGraphicsQueue; }
        [[nodiscard]] std::shared_ptr<Queue> GetSharedGraphicsQueue() const { return mGraphicsQueue; }
        [[nodiscard]] Queue& GetComputeQueue() const { return *mComputeQueue; }
        [[nodiscard]] std::shared_ptr<Queue> GetSharedComputeQueue() const { return mComputeQueue; }
        [[nodiscard]] Queue& GetTransferQueue() const { return *mTransferQueue; }
        [[nodiscard]] std::shared_ptr<Queue> GetSharedTransferQueue() const { return mTransferQueue; }
        [[nodiscard]] VkSurfaceKHR GetSurface() const { return mSurface; }

        [[nodiscard]] Semaphore CreateSemaphore(VkSemaphoreCreateFlags flags = {});
        [[nodiscard]] Fence CreateFence(VkFenceCreateFlags flags = {});
        [[nodiscard]] Command CreateCommand(const Queue& queue, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        [[nodiscard]] std::pair<VkImage, VmaAllocation> CreateImage(ImageType type,
                                                                    VkFormat format,
                                                                    VkExtent2D extent,
                                                                    VkImageUsageFlags usage) const;
        [[nodiscard]] VkImageView CreateImageView(VkImage image,
                                                  ImageType viewType,
                                                  VkFormat format,
                                                  VkImageAspectFlags aspectFlags) const;

        [[nodiscard]] std::tuple<VkBuffer, VmaAllocation, VmaAllocationInfo> CreateBuffer(BufferType bufferType,
                                                                                          uint32_t allocSize,
                                                                                          VkBufferUsageFlags usageFlags) const;
        [[nodiscard]] VmaAllocationInfo GetAllocationInfo(VmaAllocation allocation) const;

        [[nodiscard]] void* MapMemory(VmaAllocation allocation) const;
        void UnmapMemory(VmaAllocation allocation) const;

        [[nodiscard]] VkDescriptorPool CreateDescriptorPool(uint32_t maxSets, ArrayProxy<VkDescriptorPoolSize> poolSizes) const;
        [[nodiscard]] VkDescriptorSetLayout CreateDescriptorSetLayout(ArrayProxy<VkDescriptorSetLayoutBinding> bindings) const;
        [[nodiscard]] VkDescriptorSet AllocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) const;
        void UpdateDescriptorImage(VkSampler sampler, VkImageView view, VkDescriptorSet set, uint32_t arrayIndex) const;

        [[nodiscard]] VkShaderModule CreateShaderModule(const std::string& codePath) const;
        [[nodiscard]] VkPipeline CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const;
        [[nodiscard]] VkPipelineLayout CreatePipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const;

        void WaitForFence(VkFence fence, uint64_t timeout) const;
        void ResetFence(VkFence fence) const;

        void WaitIdle() const;

    private:
        void CreateInstance();
        void ChoosePhysicalDevice(const Window& window);
        void CreateDevice();
        void CreateQueues();
        void CreateAllocator();

        vkb::Instance mInstance;
        vkb::PhysicalDevice mPhysicalDevice;
        vkb::Device mDevice;
        VmaAllocator mAllocator{};
        
        std::shared_ptr<Queue> mGraphicsQueue;
        std::shared_ptr<Queue> mComputeQueue;
        std::shared_ptr<Queue> mTransferQueue;

        VkSurfaceKHR mSurface{};
        
    };
}  // namespace FS::VK