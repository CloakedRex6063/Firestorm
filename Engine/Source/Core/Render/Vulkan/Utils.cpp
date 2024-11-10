#include "Core/Render/Vulkan/Utils.h"

#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Render/Window.h"
#include "vulkan/vulkan_raii.hpp"

namespace FS::VK
{
    std::unique_ptr<vk::raii::Instance> Utils::CreateInstance(const FS::Window& window)
    {
        const vk::raii::Context context;

        constexpr auto appInfo = vk::ApplicationInfo()
                                 .setApiVersion(VK_API_VERSION_1_3)
                                 .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                                 .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                                 .setPEngineName("Firestorm")
                                 .setPApplicationName("Sandbox");

        auto requiredExtensions = window.GetRequiredExtensions();
        std::vector<const char*> layers;
        
#ifdef FS_DEBUG
        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        layers.emplace_back("VK_LAYER_KHRONOS_validation" );
#endif

        const auto instanceInfo = vk::InstanceCreateInfo()
                                  .setPApplicationInfo(&appInfo)
                                  .setPEnabledLayerNames(layers)
                                  .setPEnabledExtensionNames(requiredExtensions);
            
        return std::make_unique<vk::raii::Instance>(context, instanceInfo);
    }

    std::unique_ptr<vk::raii::PhysicalDevice> Utils::ChoosePhysicalDevice(const vk::raii::Instance& instance)
    {
        const auto devices= instance.enumeratePhysicalDevices();
        const std::vector requiredExtensions =
        {
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
        };
        
        for (const auto& device : devices)
        {
            //TODO: Error Log and exit
            if (!HasRequiredExtensions(device, requiredExtensions)) throw std::runtime_error("Required extensions not present");
            
            if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return std::make_unique<vk::raii::PhysicalDevice>(device);
            }
        }
        // TODO: Error log and exit
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    std::unique_ptr<vk::raii::Device> Utils::CreateDevice(const vk::raii::PhysicalDevice& physicalDevice,
                                                          uint32_t queueFamily)
    {
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);

        auto sync2 = vk::PhysicalDeviceSynchronization2Features().setSynchronization2(true);
        auto dynamicRendering = vk::PhysicalDeviceDynamicRenderingFeatures().setDynamicRendering(true);
        auto bufferDeviceAddressFeatures = vk::PhysicalDeviceBufferDeviceAddressFeatures().setBufferDeviceAddress(true);
        auto descriptorIndexing = vk::PhysicalDeviceDescriptorIndexingFeatures()
                                      .setDescriptorBindingPartiallyBound(true)
                                      .setDescriptorBindingVariableDescriptorCount(true)
                                      .setRuntimeDescriptorArray(true);
        
        auto deviceFeature = vk::PhysicalDeviceFeatures2().setPNext(&sync2);
        sync2.setPNext(&dynamicRendering);
        dynamicRendering.setPNext(&bufferDeviceAddressFeatures);
        bufferDeviceAddressFeatures.setPNext(&descriptorIndexing);
        
        const std::vector deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
        };
        const auto createInfo = vk::DeviceCreateInfo()
                                .setPNext(&deviceFeature)
                                .setQueueCreateInfos(queueCreateInfos)
                                .setEnabledExtensionCount(deviceExtensions.size())
                                .setPEnabledExtensionNames(deviceExtensions);
        return std::make_unique<vk::raii::Device>(physicalDevice, createInfo);
    }

    vk::ImageSubresourceRange Utils::GetImageSubresourceRange(const vk::ImageAspectFlags aspectFlags)
    {
        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(aspectFlags);
        subresourceRange.setLevelCount(vk::RemainingMipLevels);
        subresourceRange.setLayerCount(vk::RemainingArrayLayers);
        return subresourceRange;
    }

    vk::SemaphoreSubmitInfo Utils::GetSemaphoreSubmitInfo(const vk::PipelineStageFlags2 stageMask,
        const vk::Semaphore semaphore)
    {
        const auto submitInfo = vk::SemaphoreSubmitInfo().setSemaphore(semaphore).setStageMask(stageMask);
        return submitInfo;
    }

    VmaAllocationCreateInfo Utils::ImageAllocInfo(const VmaMemoryUsage usage, const VkFlags requiredFlags)
    {
        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.usage = usage;
        allocationInfo.requiredFlags = requiredFlags;
        return allocationInfo;
    }

    bool Utils::HasRequiredExtensions(const vk::raii::PhysicalDevice& physicalDevice,
                                      const std::vector<const char*>& requiredExtensions)
    {
        const std::vector<vk::ExtensionProperties> availableExtensions =
            physicalDevice.enumerateDeviceExtensionProperties();

        for (const auto &required : requiredExtensions)
        {
            bool found = false;
            for (const auto &extension : availableExtensions)
            {
                if (std::strcmp(extension.extensionName, required) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return true;
    }
}
