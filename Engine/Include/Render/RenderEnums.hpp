#pragma once

namespace FS
{
    enum class DevicePreference : u8
    {
        eHighPerformance,
        ePowerSaving,
    };

    enum class QueueType : u8
    {
        eGraphics,
        eTransfer,
    };

    enum class Format : u8
    {
        eUnknown = 0,
        eR32G32B32A32_FLOAT = 2,
        eR32G32B32A32_UINT = 3,
        eR32G32B32A32_SINT = 4,
        eR32G32B32_FLOAT = 6,
        eR32G32B32_UINT = 7,
        eR32G32B32_SINT = 8,
        eR16G16B16A16_FLOAT = 10,
        eR16G16B16A16_UNORM = 11,
        eR16G16B16A16_UINT = 12,
        eR16G16B16A16_SNORM = 13,
        eR16G16B16A16_SINT = 14,
        eR32G32_FLOAT = 16,
        eR32G32_UINT = 17,
        eR32G32_SINT = 18,
        eR10G10B10A2_UNORM = 24,
        eR10G10B10A2_UINT = 25,
        eR11G11B10_FLOAT = 26,
        eR8G8B8A8_UNORM = 28,
        eR8G8B8A8_UNORM_SRGB = 29,
        eR8G8B8A8_UINT = 30,
        eR8G8B8A8_SNORM = 31,
        eR8G8B8A8_SINT = 32,
        eR16G16_FLOAT = 34,
        eR16G16_UNORM = 35,
        eR16G16_UINT = 36,
        eR16G16_SNORM = 37,
        eR16G16_SINT = 38,
        eD32_FLOAT = 40,
        eR32_FLOAT = 41,
        eR32_UINT = 42,
        eR32_SINT = 43,
        eD24_UNORM_S8_UINT = 45,
        eR8G8_UNORM = 49,
        eR8G8_UINT = 50,
        eR8G8_SNORM = 51,
        eR8G8_SINT = 52,
        eR16_FLOAT = 54,
        eD16_UNORM = 55,
        eR16_UNORM = 56,
        eR16_UINT = 57,
        eR16_SNORM = 58,
        eR16_SINT = 59,
        eR8_UNORM = 61,
        eR8_UINT = 62,
        eR8_SNORM = 63,
        eR8_SINT = 64,
        eA8_UNORM = 65,
        eR1_UNORM = 66,
        eR8G8_B8G8_UNORM = 68,
        eG8R8_G8B8_UNORM = 69,
        eBC1_UNORM = 71,
        eBC1_UNORM_SRGB = 72,
        eBC2_UNORM = 74,
        eBC2_UNORM_SRGB = 75,
        eBC3_UNORM = 77,
        eBC3_UNORM_SRGB = 78,
        eBC4_UNORM = 80,
        eBC4_SNORM = 81,
        eBC5_UNORM = 83,
        eBC5_SNORM = 84,
        eB5G6R5_UNORM = 85,
        eB5G5R5A1_UNORM = 86,
        eB8G8R8A8_UNORM = 87,
        eB8G8R8A8_UNORM_SRGB = 91,
        eBC6H_UF16 = 95,
        eBC6H_SF16 = 96,
        eBC7_UNORM = 98,
        eBC7_UNORM_SRGB = 99,
    };

    enum class PrimitiveTopology : u8
    {
        eUndefined = 0,
        ePoint = 1,
        eLine = 2,
        eTriangle = 3,
    };

    enum class FillMode : u8
    {
        eWireframe = 2,
        eSolid = 3,
    };

    enum class CullMode : u8
    {
        eNone = 1,
        eFront = 2,
        eBack = 3
    };

    enum class FrontFace : u8
    {
        eClockwise = 0,
        eCounterClockwise = 1
    };

    enum class ViewType : u8
    {
        eUnknown = 0,
        eBuffer = 1,
        eTexture1D = 2,
        eTexture1DArray = 3,
        eTexture2D = 4,
        eTexture2DArray = 5,
        eTexture3D = 8
    };

    enum class RenderViewType : u8
    {
        eTexture2D = 4,
        eTexture2DArray = 5,
        eTexture3D = 8,
    };

    enum class BufferType : u8
    {
        eStorage,
        eUniform,
        eStaging,
        eReadback
    };
} // namespace FS