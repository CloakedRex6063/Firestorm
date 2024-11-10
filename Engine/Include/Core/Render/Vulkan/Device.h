#pragma once

namespace FS
{
    class Window;
}

namespace FS::VK
{

    class Device
    {
    public:
        Device(const Window& window);

        [[nodiscard]] VmaAllocator& GetAllocator() const { return *mAllocator; }

        [[nodiscard]] vk::raii::Instance& GetInstance() const { return *mInstance; }

        [[nodiscard]] vk::raii::PhysicalDevice& GetPhysicalDevice() const { return *mPhysicalDevice; }

        [[nodiscard]] operator vk::raii::Device&() const { return *mDevice; }

        [[nodiscard]] std::unique_ptr<vk::raii::CommandPool> CreateCommandPool(uint32_t queueFamilyIndex) const;

        [[nodiscard]] std::unique_ptr<vk::raii::CommandBuffer> CreateCommandBuffer(
            const vk::raii::CommandPool& commandPool) const;

        [[nodiscard]] vk::raii::Semaphore CreateSemaphore() const;

        [[nodiscard]] vk::raii::Fence CreateFence(vk::FenceCreateFlags flags) const;

        [[nodiscard]] vk::raii::SwapchainKHR CreateSwapchainKHR(
            const vk::raii::SurfaceKHR& surface, const glm::uvec2& size,
            const vk::raii::SwapchainKHR& oldSwapchain = nullptr) const;

        [[nodiscard]] std::unique_ptr<vk::Image> CreateImage(vk::Extent2D extent, vk::Format format,
                                                             vk::ImageUsageFlags usageFlags,
                                                             const VmaAllocationCreateInfo& allocInfo,
                                                             VmaAllocation& allocation) const;

        [[nodiscard]] std::unique_ptr<vk::raii::ImageView> CreateImageView(const vk::Image& image, vk::Format format,
                                                                           vk::ImageAspectFlags aspectFlags) const;

        [[nodiscard]] std::unique_ptr<vk::raii::ShaderModule> CreateShaderModule(const std::vector<char>& code) const;
        [[nodiscard]] std::pair<vk::PipelineShaderStageCreateInfo, vk::raii::ShaderModule> CreateShaderStage(
            vk::ShaderStageFlagBits flags, const std::string& path) const;

        [[nodiscard]] std::unique_ptr<vk::raii::PipelineLayout> CreatePipelineLayout(
            const vk::PipelineLayoutCreateInfo& createInfo) const;

        [[nodiscard]] std::unique_ptr<vk::raii::Pipeline> CreateGraphicsPipeline(
            const vk::GraphicsPipelineCreateInfo& createInfo) const;

        [[nodiscard]] std::unique_ptr<vk::raii::Pipeline> CreateComputePipeline(
            const vk::ComputePipelineCreateInfo& createInfo) const;

        [[nodiscard]] uint32_t GetQueueFamily(vk::QueueFlags queueType) const;

        void WaitForFence(const vk::raii::Fence& fence, uint64_t timeout) const;

        void ResetFence(const vk::raii::Fence& fence) const;

        void WaitIdle() const;

    private:
        struct AllocatorDeleter
        {
            void operator()(const VmaAllocator* alloc) const;
        };

        [[nodiscard]] std::unique_ptr<VmaAllocator, AllocatorDeleter> CreateAllocator() const;

        std::unique_ptr<vk::raii::Instance> mInstance;
        std::unique_ptr<vk::raii::Device> mDevice;
        std::unique_ptr<vk::raii::PhysicalDevice> mPhysicalDevice;
        std::unique_ptr<VmaAllocator, AllocatorDeleter> mAllocator;
    };
} // namespace FS::VK
