#pragma once

namespace FS::VK::Constants
{
    constexpr uint32_t MaxFramesInFlight = 3;
    constexpr uint16_t MaxUniformDescriptors = std::numeric_limits<uint16_t>::max();
    constexpr uint8_t UniformBinding = 0;
    constexpr uint16_t MaxSamplerDescriptors = std::numeric_limits<uint16_t>::max();
    constexpr uint8_t SamplerBinding = 1;
    constexpr uint16_t MaxStorageDescriptors = std::numeric_limits<uint16_t>::max();
    constexpr uint8_t StorageBinding = 2;
    constexpr uint16_t MaxImageDescriptors = std::numeric_limits<uint16_t>::max();
    constexpr uint8_t ImageBinding = 3;

}
