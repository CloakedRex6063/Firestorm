#pragma once
#include "Tools/Enums.h"

namespace FS
{
    struct Vertex
    {
        glm::vec3 mPosition{};
        float mUVx{};
        glm::vec3 mNormal{};
        float mUVy{};
        glm::vec4 mColor{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mPosition, mUVx, mNormal, mUVy, mColor);
        }
    };

    struct Node
    {
        glm::mat4 mTransform{};
        int mMeshIndex{};
        int mLightIndex{};
        std::vector<uint32_t> mChildren{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mTransform, mMeshIndex, mLightIndex, mChildren);
        }
    };

    struct Material
    {
        glm::vec4 mBaseColorFactor{};
        float mMetallicFactor{};
        float mRoughnessFactor{};
        int mBaseTextureIndex{};
        int mRoughnessTextureIndex{};
        int mOcclusionTextureIndex{};
        float mAO{};
        int mEmissiveTextureIndex{};
        glm::vec3 mEmissiveFactor{};
        AlphaMode mAlphaMode{};
        bool mDoubleSided{};
        float mAlphaCutoff{};
        float mIOR{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mBaseColorFactor,
                    mMetallicFactor,
                    mRoughnessFactor,
                    mBaseTextureIndex,
                    mRoughnessTextureIndex,
                    mAO,
                    mEmissiveTextureIndex,
                    mEmissiveFactor,
                    mAlphaMode,
                    mAlphaCutoff,
                    mIOR);
        }
    };

    struct Mesh
    {
        uint32_t mVertexOffset{};
        uint32_t mIndexOffset{};
        uint32_t mIndexCount{};
        int mMaterialIndex{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mVertexOffset, mIndexOffset, mIndexCount, mMaterialIndex);
        }
    };

    struct Texture
    {
        int mSamplerIndex{};
        int mImageIndex{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mSamplerIndex, mImageIndex);
        }
    };

    struct Sampler
    {
        TextureFilter mMinFilter{};
        TextureFilter mMagFilter{};
        TextureWrap mWrapS{};
        TextureWrap mWrapT{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mMinFilter, mMagFilter, mWrapS, mWrapT);
        }
    };

    struct Image
    {
        int mWidth{};
        int mHeight{};
        std::vector<uint8_t> mPixels{};
    };

    struct Light
    {
        LightType mType{};
        glm::vec3 mColor{};
        float mIntensity{};
        float mRange{};
        float mInnerConeAngle{};
        float mOuterConeAngle{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mType, mColor, mIntensity, mRange, mInnerConeAngle, mOuterConeAngle);
        }
    };

    struct Model
    {
        std::vector<uint32_t> mRootNodes{};
        std::vector<Node> mNodes{};
        std::vector<Mesh> mMeshes{};
        std::vector<Material> mMaterials{};
        std::vector<Texture> mTextures{};
        std::vector<Sampler> mSamplers{};
        std::vector<Image> mImages{};
        std::vector<Light> mLights{};

        std::vector<Vertex> mVertices{};
        std::vector<uint32_t> mIndices{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mRootNodes, mNodes, mMeshes, mMaterials, mTextures, mSamplers, mLights, mVertices, mIndices);
        }
    };

    struct ResourceHandle
    {
        std::shared_ptr<std::string> mResourceReference;

        bool operator==(const ResourceHandle& other) const
        {
            if (mResourceReference == other.mResourceReference)
            {
                return true;
            }
            return mResourceReference && other.mResourceReference && *mResourceReference == *other.mResourceReference;
        }
    };
}  // namespace FS

template <>
struct std::hash<FS::ResourceHandle>
{
    size_t operator()(const FS::ResourceHandle& handle) const noexcept
    {
        return handle.mResourceReference ? std::hash<std::string>{}(*handle.mResourceReference) : 0;
    }
};