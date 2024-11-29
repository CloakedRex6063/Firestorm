#pragma once

namespace FS
{
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
        eMappedStorage,
        eMappedUniform,
        eCPU,
    };

    enum class ImageLayout : uint8_t
    {
        eUndefined,
        eGeneral,
        eColorAttachment,
        eDepthStencilAttachment,
        eDepthStencilReadOnly,
        eShaderReadOnly,
        eTransferSrc,
        eTransferDst,
        ePreInitialised,
        eDepthReadOnlyStencilAttachment,
        eDepthAttachmentStencilReadOnly,
        eDepthAttachment,
        eDepthReadOnly,
        eStencilAttachment,
        eStencilReadOnly,
        eReadOnly,
        eAttachment,
        ePresent,
    };

    enum class MaterialType : uint8_t
    {
        eOpaque,
        eTransparent
    };
}  // namespace FS