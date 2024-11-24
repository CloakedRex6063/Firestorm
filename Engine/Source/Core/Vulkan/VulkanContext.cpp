#include "Core/Render/Vulkan/VulkanContext.h"
#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Render/Vulkan/VulkanCommand.h"
#include "Core/Render/Vulkan/VulkanQueue.h"
#include "Core/Render/Vulkan/VulkanSync.h"
#include "Core/Render/Vulkan/Tools/VulkanUtils.h"
#include "Core/Render/Window.h"
#include "Core/Render/Vulkan/Constants.hpp"

namespace FS
{
    VulkanContext::VulkanContext(const Window& window)
    {
        CreateInstance();
        ChoosePhysicalDevice(window);
        CreateDevice();
        CreateQueues();
        CreateAllocator();
    }

    VulkanContext::~VulkanContext()
    {
        vkb::destroy_surface(mInstance, mSurface);
        vmaDestroyAllocator(mAllocator);
        vkb::destroy_device(mDevice);
        vkb::destroy_instance(mInstance);
    }

    VkBool32 DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                           const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                           void* userData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Log::Warn("{}, {}", callbackData->pMessage, userData);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Log::Error("{}, {}", callbackData->pMessage, userData);
                break;
            default: ;
                Log::Info("{}, {}", callbackData->pMessage, userData);
                break;
        }
        return true;
    }

    void VulkanContext::CreateInstance()
    {
        vkb::InstanceBuilder instanceBuilder;
        instanceBuilder.set_app_name("Sandbox").set_engine_name("Firestorm").require_api_version(VK_API_VERSION_1_3);
#ifdef FS_DEBUG
        instanceBuilder.set_debug_callback(DebugCallback).request_validation_layers();
#endif
        auto instanceReturn = instanceBuilder.build();
        ASSERT(instanceReturn.has_value());
        mInstance = instanceReturn.value();
    }

    void VulkanContext::ChoosePhysicalDevice(const Window& window)
    {
        mSurface = window.CreateSurface(mInstance);
        vkb::PhysicalDeviceSelector selector(mInstance);
        VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                                                    .descriptorIndexing = true,
                                                    .descriptorBindingUniformBufferUpdateAfterBind = true,
                                                    .descriptorBindingSampledImageUpdateAfterBind = true,
                                                    .descriptorBindingStorageImageUpdateAfterBind = true,
                                                    .descriptorBindingStorageBufferUpdateAfterBind = true,
                                                    .descriptorBindingPartiallyBound = true,
                                                    .descriptorBindingVariableDescriptorCount = true,
                                                    .runtimeDescriptorArray = true,
                                                    .bufferDeviceAddress = true};
        VkPhysicalDeviceVulkan13Features features13{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
                                                    .synchronization2 = true,
                                                    .dynamicRendering = true};
        auto physicalDeviceReturn = selector.set_minimum_version(1, 3)
                                            .set_surface(mSurface)
                                            .set_required_features_12(features12)
                                            .set_required_features_13(features13)
                                            .require_separate_transfer_queue()
                                            .require_separate_compute_queue()
                                            .add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                            .add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
                                            .add_required_extension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)
                                            .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                                            .select();
        ASSERT(physicalDeviceReturn.has_value());
        mPhysicalDevice = physicalDeviceReturn.value();
    }

    void VulkanContext::CreateDevice()
    {
        const auto deviceReturn = vkb::DeviceBuilder(mPhysicalDevice).build();
        if (!deviceReturn)
        {
            Log::Critical("Failed to create device {}", deviceReturn.error().message());
        }
        mDevice = deviceReturn.value();
    }

    void VulkanContext::CreateQueues()
    {
        const auto graphicsFamily = mDevice.get_queue_index(vkb::QueueType::graphics).value();
        const auto graphicsQueue = mDevice.get_queue(vkb::QueueType::graphics).value();
        mGraphicsQueue = std::make_unique<VulkanQueue>(graphicsQueue, graphicsFamily);

        auto transferQueue = mDevice.get_queue(vkb::QueueType::transfer).value();
        auto transferFamily = mDevice.get_queue_index(vkb::QueueType::transfer).value();
        mTransferQueue = std::make_unique<VulkanQueue>(transferQueue, transferFamily);

        auto computeQueue = mDevice.get_queue(vkb::QueueType::compute).value();
        auto computeFamily = mDevice.get_queue_index(vkb::QueueType::compute).value();
        mComputeQueue = std::make_unique<VulkanQueue>(computeQueue, computeFamily);
    }

    void VulkanContext::CreateAllocator()
    {
        const VmaAllocatorCreateInfo allocatorInfo{.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
                                                   .physicalDevice = mPhysicalDevice,
                                                   .device = mDevice,
                                                   .instance = mInstance};
        vmaCreateAllocator(&allocatorInfo, &mAllocator);
    }

    Semaphore VulkanContext::CreateSemaphore(const VkSemaphoreCreateFlags flags) { return {shared_from_this(), flags}; }

    Fence VulkanContext::CreateFence(const VkFenceCreateFlags flags) { return {shared_from_this(), flags}; }

    VulkanCommand VulkanContext::CreateCommand(const VulkanQueue& queue, const VkCommandBufferLevel level)
    {
        const VkCommandPoolCreateInfo poolCreateInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                                        .queueFamilyIndex = queue.GetFamilyIndex()};
        VkCommandPool commandPool;
        vkCreateCommandPool(mDevice, &poolCreateInfo, nullptr, &commandPool);
        const VkCommandBufferAllocateInfo cmdAllocateInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                             .commandPool = commandPool,
                                                             .level = level,
                                                             .commandBufferCount = 1};
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(mDevice, &cmdAllocateInfo, &commandBuffer);
        return VulkanCommand{shared_from_this(), commandPool, commandBuffer};
    }

    std::pair<VkImage, VmaAllocation> VulkanContext::CreateImage(const ImageType type,
                                                                 const VkFormat format,
                                                                 const VkExtent2D extent,
                                                                 const VkImageUsageFlags usage) const
    {
        VkImageType vkType;
        switch (type)
        {
            case ImageType::e1D:
                vkType = VK_IMAGE_TYPE_1D;
                break;
            case ImageType::e2D:
                vkType = VK_IMAGE_TYPE_2D;
                break;
            case ImageType::e3D:
                vkType = VK_IMAGE_TYPE_3D;
                break;
            default:
                vkType = VK_IMAGE_TYPE_2D;
        }
        const VkImageCreateInfo imageCreateInfo = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                                   .imageType = vkType,
                                                   .format = format,
                                                   .extent = VkExtent3D(extent.width, extent.height, 1),
                                                   .mipLevels = 1,
                                                   .arrayLayers = 1,
                                                   .samples = VK_SAMPLE_COUNT_1_BIT,
                                                   .tiling = VK_IMAGE_TILING_OPTIMAL,
                                                   .usage = usage};
        VkImage image;
        VmaAllocation allocation;
        constexpr VmaAllocationCreateInfo allocCreateInfo{.usage = VMA_MEMORY_USAGE_GPU_ONLY,
                                                          .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
        vmaCreateImage(mAllocator, &imageCreateInfo, &allocCreateInfo, &image, &allocation, nullptr);
        return {image, allocation};
    }

    VkImageView VulkanContext::CreateImageView(VkImage image,
                                               const ImageType viewType,
                                               const VkFormat format,
                                               const VkImageAspectFlags aspectFlags) const
    {
        VkImageViewType vkType;
        switch (viewType)
        {
            case ImageType::e1D:
                vkType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case ImageType::e2D:
                vkType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case ImageType::e3D:
                vkType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                vkType = VK_IMAGE_VIEW_TYPE_2D;
                break;
        }
        const VkImageViewCreateInfo imageInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                              .image = image,
                                              .viewType = vkType,
                                              .format = format,
                                              .components{VK_COMPONENT_SWIZZLE_IDENTITY,
                                                          VK_COMPONENT_SWIZZLE_IDENTITY,
                                                          VK_COMPONENT_SWIZZLE_IDENTITY,
                                                          VK_COMPONENT_SWIZZLE_IDENTITY},
                                              .subresourceRange = VulkanUtils::GetSubresourceRange(aspectFlags)};
        VkImageView imageView;
        vkCreateImageView(mDevice, &imageInfo, nullptr, &imageView);
        return imageView;
    }

    std::tuple<VkBuffer, VmaAllocation, VmaAllocationInfo> VulkanContext::CreateBuffer(const BufferType bufferType,
        const uint32_t allocSize,
        const VkBufferUsageFlags usageFlags) const
    {
        VkBufferUsageFlags mainUsageFlags = 0;
        VmaMemoryUsage memoryUsage{};
        VkMemoryPropertyFlags requiredMemoryFlags = 0;
        switch (bufferType)
        {
            case BufferType::eStaging:
                mainUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
                requiredMemoryFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            case BufferType::eIndex:
                mainUsageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
                break;
            case BufferType::eVertex:
            case BufferType::eGPU:
                mainUsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
                break;
            case BufferType::eCPU:
                mainUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
                requiredMemoryFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        const VkBufferCreateInfo bufferCreateInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                     .size = allocSize,
                                                     .usage = mainUsageFlags | usageFlags,
                                                     .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
        VkBuffer buffer;
        const VmaAllocationCreateInfo createInfo{.usage = memoryUsage, .requiredFlags = requiredMemoryFlags};
        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        vmaCreateBuffer(mAllocator, &bufferCreateInfo, &createInfo, &buffer, &allocation, &allocInfo);
        return {buffer, allocation, allocInfo};
    }

    VmaAllocationInfo VulkanContext::GetAllocationInfo(VmaAllocation allocation) const
    {
        VmaAllocationInfo allocationInfo{};
        vmaGetAllocationInfo(mAllocator, allocation, &allocationInfo);
        return allocationInfo;
    }

    void* VulkanContext::MapMemory(VmaAllocation allocation) const
    {
        void* mappedMemory = nullptr;
        vmaMapMemory(mAllocator, allocation, &mappedMemory);
        return mappedMemory;
    }

    void VulkanContext::UnmapMemory(VmaAllocation allocation) const { vmaUnmapMemory(mAllocator, allocation); }

    VkDescriptorPool VulkanContext::CreateDescriptorPool(const uint32_t maxSets,
                                                         ArrayProxy<VkDescriptorPoolSize> poolSizes) const
    {
        const VkDescriptorPoolCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                                       .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
                                                       .maxSets = maxSets,
                                                       .poolSizeCount = poolSizes.size(),
                                                       .pPoolSizes = poolSizes.data()};
        VkDescriptorPool descriptorPool;
        vkCreateDescriptorPool(mDevice, &createInfo, nullptr, &descriptorPool);
        return descriptorPool;
    }

    VkDescriptorSetLayout VulkanContext::CreateDescriptorSetLayout(
        const ArrayProxy<VkDescriptorSetLayoutBinding> bindings) const
    {
        constexpr VkDescriptorBindingFlags flags =
            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        std::vector bindingsFlags(bindings.size(), flags);
        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingsInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount = bindings.size(),
            .pBindingFlags = bindingsFlags.data()};

        const VkDescriptorSetLayoutCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                            .pNext = &bindingsInfo,
                                                            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                                                            .bindingCount = bindings.size(),
                                                            .pBindings = bindings.data()};
        VkDescriptorSetLayout descriptorSetLayout;
        vkCreateDescriptorSetLayout(mDevice, &createInfo, nullptr, &descriptorSetLayout);
        return descriptorSetLayout;
    }

    VkDescriptorSet VulkanContext::AllocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) const
    {
        const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                       .descriptorPool = pool,
                                                       .descriptorSetCount = 1,
                                                       .pSetLayouts = &layout};
        VkDescriptorSet descriptorSet;
        vkAllocateDescriptorSets(mDevice, &allocInfo, &descriptorSet);
        return descriptorSet;
    }

    void VulkanContext::UpdateDescriptorImage(VkSampler sampler,
                                              VkImageView view,
                                              VkDescriptorSet set,
                                              const uint32_t arrayIndex) const
    {
        VkDescriptorImageInfo imageInfo{.sampler = sampler,
                                        .imageView = view,
                                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        const VkWriteDescriptorSet writeInfo = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = Constants::SamplerBinding,
            .dstArrayElement = arrayIndex,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(mDevice, 1, &writeInfo, 0, nullptr);
    };

    VkShaderModule VulkanContext::CreateShaderModule(const std::string& codePath) const
    {
        const auto code = gEngine.FileSystem().ReadBinaryFile(Directory::eNone, codePath);
        const VkShaderModuleCreateInfo shaderModuleInfo = {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                                           .codeSize = code.size(),
                                                           .pCode = reinterpret_cast<const uint32_t*>(code.data())};
        VkShaderModule shaderModule;
        vkCreateShaderModule(mDevice, &shaderModuleInfo, nullptr, &shaderModule);
        return shaderModule;
    }

    VkPipeline VulkanContext::CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const
    {
        VkPipeline pipeline;
        vkCreateGraphicsPipelines(mDevice, nullptr, 1, &createInfo, nullptr, &pipeline);
        return pipeline;
    }

    VkPipelineLayout VulkanContext::CreatePipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const
    {
        VkPipelineLayout pipelineLayout;
        vkCreatePipelineLayout(mDevice, &createInfo, nullptr, &pipelineLayout);
        return pipelineLayout;
    }

    void VulkanContext::WaitForFence(VkFence fence, const uint64_t timeout) const
    {
        vkWaitForFences(mDevice, 1, &fence, true, timeout);
    }

    void VulkanContext::ResetFence(VkFence fence) const { vkResetFences(mDevice, 1, &fence); }

    void VulkanContext::WaitIdle() const { vkDeviceWaitIdle(mDevice); }
} // namespace FS