#pragma once

namespace FS
{
    enum class CameraType : uint8_t
    {
        ePerspective,
        eOrthographic,
    };

    enum class AlphaMode
    {
        eOpaque,
        eTransparent
    };

    enum class TextureFilter : uint8_t
    {
        eNearest,
        eLinear,
        eNearestMipmapNearest,
        eLinearMipmapNearest,
        eNearestMipmapLinear,
        eLinearMipmapLinear,
    };

    enum class TextureWrap : uint8_t
    {
        eRepeat,
        eMirroredRepeat,
        eClampToEdge
    };

    enum class LightType : uint8_t
    {
        ePoint,
        eDirectional,
        eSpot
    };
}