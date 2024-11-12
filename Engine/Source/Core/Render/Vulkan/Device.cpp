#include "Core/Render/Vulkan/Device.h"
#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Render/Vulkan/Utils.h"

namespace FS::VK
{
    Device::Device(const FS::Window& window)
    {
        mInstance = Utils::CreateInstance(window);
        mPhysicalDevice = Utils::ChoosePhysicalDevice(GetInstance());
        mDevice = Utils::CreateDevice(GetPhysicalDevice(), GetQueueFamily(vk::QueueFlagBits::eGraphics));
        mAllocator = CreateAllocator();
    }

    std::unique_ptr<vk::raii::CommandPool> Device::CreateCommandPool(const uint32_t queueFamilyIndex) const
    {
        const auto createInfo = vk::CommandPoolCreateInfo()
                                    .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                    .setQueueFamilyIndex(queueFamilyIndex);
        return std::make_unique<vk::raii::CommandPool>(mDevice->createCommandPool(createInfo));
    }

    std::unique_ptr<vk::raii::CommandBuffer> Device::CreateCommandBuffer(const vk::raii::CommandPool& commandPool) const
    {
        const auto allocateInfo = vk::CommandBufferAllocateInfo()
                                      .setCommandPool(commandPool)
                                      .setLevel(vk::CommandBufferLevel::ePrimary)
                                      .setCommandBufferCount(1);
        auto commandBuffer = std::move(mDevice->allocateCommandBuffers(allocateInfo).front());
        return std::make_unique<vk::raii::CommandBuffer>(std::move(commandBuffer));
    }

    vk::raii::Semaphore Device::CreateSemaphore() const { return mDevice->createSemaphore(vk::SemaphoreCreateInfo()); }

    vk::raii::Fence Device::CreateFence(const vk::FenceCreateFlags flags) const
    {
        return mDevice->createFence(vk::FenceCreateInfo().setFlags(flags));
    }

    vk::raii::SwapchainKHR Device::CreateSwapchainKHR(const vk::raii::SurfaceKHR& surface, const glm::uvec2& size,
                                                      const vk::raii::SwapchainKHR& oldSwapchain) const
    {
        const auto capabilities = mPhysicalDevice->getSurfaceCapabilitiesKHR(surface);
        auto formats = mPhysicalDevice->getSurfaceFormatsKHR(surface);
        const auto imageCount = std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);
        const auto createInfo =
            vk::SwapchainCreateInfoKHR()
                .setSurface(surface)
                .setMinImageCount(imageCount)
                .setImageFormat(vk::Format::eB8G8R8A8Unorm)
                .setImageExtent(vk::Extent2D(size.x, size.y))
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
                .setImageSharingMode(vk::SharingMode::eExclusive)
                .setPresentMode(vk::PresentModeKHR::eMailbox)
                .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
                .setImageArrayLayers(1)
                .setOldSwapchain(oldSwapchain);
        return mDevice->createSwapchainKHR(createInfo);
    }

    std::unique_ptr<vk::Image> Device::CreateImage(const vk::Extent2D extent, const vk::Format format,
                                                   const vk::ImageUsageFlags usageFlags,
                                                   const VmaAllocationCreateInfo& allocInfo,
                                                   VmaAllocation& allocation) const
    {

        const VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
                                                      .setImageType(vk::ImageType::e2D)
                                                      .setFormat(format)
                                                      .setExtent(vk::Extent3D(extent, 1))
                                                      .setMipLevels(1)
                                                      .setArrayLayers(1)
                                                      .setSamples(vk::SampleCountFlagBits::e1)
                                                      .setTiling(vk::ImageTiling::eOptimal)
                                                      .setUsage(usageFlags);
        ;
        VkImage image;
        vmaCreateImage(GetAllocator(), &imageCreateInfo, &allocInfo, &image, &allocation, nullptr);
        return std::make_unique<vk::Image>(image);
    }

    std::unique_ptr<vk::raii::ImageView> Device::CreateImageView(const vk::Image& image, const vk::Format format,
                                                                 const vk::ImageAspectFlags aspectFlags) const
    {
        const auto subresourceRange =
            vk::ImageSubresourceRange().setAspectMask(aspectFlags).setLevelCount(1).setLayerCount(1);

        const auto createInfo = vk::ImageViewCreateInfo()
                                    .setViewType(vk::ImageViewType::e2D)
                                    .setImage(image)
                                    .setFormat(format)
                                    .setSubresourceRange(subresourceRange);

        return std::make_unique<vk::raii::ImageView>(mDevice->createImageView(createInfo));
    }

    std::unique_ptr<vk::Buffer> Device::CreateBuffer(const vk::DeviceSize allocSize, const vk::BufferUsageFlags usage,
                                                     const VmaMemoryUsage memoryUsage, VmaAllocation& allocation) const
    {
        const VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo().setSize(allocSize).setUsage(usage);

        VmaAllocationCreateInfo createInfo{};
        createInfo.usage = memoryUsage;
        createInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer buffer;
        vmaCreateBuffer(GetAllocator(), &bufferCreateInfo, &createInfo, &buffer, &allocation, nullptr);
        return std::make_unique<vk::Buffer>(buffer);
    }

    vk::DeviceAddress Device::GetBufferAddress(const vk::Buffer& buffer) const
    {
        const auto addressInfo = vk::BufferDeviceAddressInfo().setBuffer(buffer);
        return mDevice->getBufferAddress(addressInfo);
    }

    void* Device::MapMemory(const VmaAllocation& allocation) const
    {
        void* mapped;
        vmaMapMemory(GetAllocator(), allocation, &mapped);
        return mapped;
    }

    void Device::UnmapMemory(const VmaAllocation& allocation) const { vmaUnmapMemory(GetAllocator(), allocation); }

    std::unique_ptr<vk::raii::ShaderModule> Device::CreateShaderModule(const std::vector<char>& code) const
    {
        const auto* codePtr = reinterpret_cast<const uint32_t*>(code.data());

        const auto shaderCreateInfo = vk::ShaderModuleCreateInfo().setPCode(codePtr).setCodeSize(code.size());
        return std::make_unique<vk::raii::ShaderModule>(mDevice->createShaderModule(shaderCreateInfo));
    }
    std::pair<vk::PipelineShaderStageCreateInfo, vk::raii::ShaderModule> Device::CreateShaderStage(
        const vk::ShaderStageFlagBits flags, const std::string& path) const
    {
        const auto shader = gEngine.FileSystem().ReadBinaryFile(Directory::eNone, path);
        const auto shaderModule = CreateShaderModule(shader);

        auto createInfo = vk::PipelineShaderStageCreateInfo().setModule(*shaderModule).setStage(flags).setPName("main");
        return {createInfo, std::move(*shaderModule)};
    }

    std::unique_ptr<vk::raii::PipelineLayout> Device::CreatePipelineLayout(
        const vk::PipelineLayoutCreateInfo& createInfo) const
    {
        return std::make_unique<vk::raii::PipelineLayout>(mDevice->createPipelineLayout(createInfo));
    }

    std::unique_ptr<vk::raii::Pipeline> Device::CreateGraphicsPipeline(
        const vk::GraphicsPipelineCreateInfo& createInfo) const
    {
        return std::make_unique<vk::raii::Pipeline>(mDevice->createGraphicsPipeline(nullptr, createInfo));
    }

    std::unique_ptr<vk::raii::Pipeline> Device::CreateComputePipeline(
        const vk::ComputePipelineCreateInfo& createInfo) const
    {
        return std::make_unique<vk::raii::Pipeline>(mDevice->createComputePipeline(nullptr, createInfo));
    }

    uint32_t Device::GetQueueFamily(vk::QueueFlags queueType) const
    {
        const auto queueFamilies = mPhysicalDevice->getQueueFamilyProperties();
        const auto generalFamily =
            std::ranges::find_if(queueFamilies, [queueType](const vk::QueueFamilyProperties& queueFamily)
                                 { return (queueFamily.queueFlags & queueType) != vk::Flags<vk::QueueFlagBits>{}; });

        return static_cast<uint32_t>(std::distance(queueFamilies.begin(), generalFamily));
    }

    void Device::WaitForFence(const vk::raii::Fence& fence, const uint64_t timeout) const
    {
        switch (mDevice->waitForFences(*fence, true, timeout))
        {
        case vk::Result::eTimeout:
            // TODO: Log error
            break;
        default:
            break;
        }
    }

    void Device::ResetFence(const vk::raii::Fence& fence) const { mDevice->resetFences(*fence); }

    void Device::WaitIdle() const { mDevice->waitIdle(); }

    std::unique_ptr<VmaAllocator, Device::AllocatorDeleter> Device::CreateAllocator() const
    {
        auto createInfo = VmaAllocatorCreateInfo();
        createInfo.device = **mDevice;
        createInfo.physicalDevice = **mPhysicalDevice;
        createInfo.instance = **mInstance;
        createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        auto* rawAllocator = new VmaAllocator();
        vmaCreateAllocator(&createInfo, rawAllocator);
        std::unique_ptr<VmaAllocator, AllocatorDeleter> allocator(rawAllocator);
        return std::move(allocator);
    }

    void Device::AllocatorDeleter::operator()(const VmaAllocator* alloc) const
    {
        vmaDestroyAllocator(*alloc);
        delete alloc;
    }
} // namespace FS::VK