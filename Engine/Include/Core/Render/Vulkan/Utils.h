#pragma once
#include "Core/Render/Window.h"

namespace vk::raii
{
    class Device;
}

namespace FS::VK
{
    class Window;

    class Utils
    {
    public:
        static std::unique_ptr<vk::raii::Instance> CreateInstance(const FS::Window& window);
        static std::unique_ptr<vk::raii::PhysicalDevice> ChoosePhysicalDevice(const vk::raii::Instance& instance);
        static std::unique_ptr<vk::raii::Device> CreateDevice(const vk::raii::PhysicalDevice& physicalDevice,
                                                              uint32_t queueFamily);

        static vk::ImageSubresourceRange GetImageSubresourceRange(vk::ImageAspectFlags aspectFlags);
        static vk::SemaphoreSubmitInfo GetSemaphoreSubmitInfo(vk::PipelineStageFlags2 stageMask,
                                                              vk::Semaphore semaphore);
        static VmaAllocationCreateInfo ImageAllocInfo(VmaMemoryUsage usage, VkFlags requiredFlags);

    private:
        static bool HasRequiredExtensions(const vk::raii::PhysicalDevice& physicalDevice,
                                          const std::vector<const char*>& requiredExtensions);
    };
} // FS::VK
