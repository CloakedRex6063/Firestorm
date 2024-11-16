#pragma once

enum class ImageType : uint8_t
{
    e1D,
    e2D,
    e3D
};

enum class BufferType : uint8_t
{
    eStaging,
    eVertex,
    eIndex,
    eGPU,
    eCPU
};