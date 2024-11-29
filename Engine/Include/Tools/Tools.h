#pragma once
#include "Enums.h"
#include "fastgltf/core.hpp"

namespace FS
{
    class Tools
    {
    public:
        static TextureFilter GetTextureFilter(fastgltf::Filter filter);
        static TextureWrap GetTextureWrap(fastgltf::Wrap wrap);
        static LightType GetLightType(fastgltf::LightType lightType);
    };
}  // namespace FS