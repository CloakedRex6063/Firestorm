#include "Tools/Tools.h"

namespace FS
{

    TextureFilter Tools::GetTextureFilter(const fastgltf::Filter filter)
    {
        switch (filter)
        {
            case fastgltf::Filter::Nearest:
                return TextureFilter::eNearest;
            case fastgltf::Filter::Linear:
                return TextureFilter::eLinear;
            case fastgltf::Filter::NearestMipMapNearest:
                return TextureFilter::eNearestMipmapNearest;
            case fastgltf::Filter::LinearMipMapNearest:
                return TextureFilter::eLinearMipmapNearest;
            case fastgltf::Filter::NearestMipMapLinear:
                return TextureFilter::eNearestMipmapLinear;
            case fastgltf::Filter::LinearMipMapLinear:
                return TextureFilter::eLinearMipmapLinear;
            default:
                return TextureFilter::eNearest;
        }
    }
    
    TextureWrap Tools::GetTextureWrap(const fastgltf::Wrap wrap)
    {
        switch (wrap)
        {
            case fastgltf::Wrap::ClampToEdge:
                return TextureWrap::eClampToEdge;
            case fastgltf::Wrap::MirroredRepeat:
                return TextureWrap::eMirroredRepeat;
            case fastgltf::Wrap::Repeat:
                return TextureWrap::eRepeat;
            default:
                return TextureWrap::eRepeat;
        }
    }
    
    LightType Tools::GetLightType(const fastgltf::LightType lightType)
    {
        switch (lightType)
        {
            case fastgltf::LightType::Directional:
                return LightType::eDirectional;
            case fastgltf::LightType::Point:
                return LightType::ePoint;
            case fastgltf::LightType::Spot:
                return LightType::eSpot;
            default:
                return LightType::eSpot;
        }
    }
}  // namespace FS