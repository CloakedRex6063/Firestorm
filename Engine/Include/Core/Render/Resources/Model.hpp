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
        glm::vec3 mTangent{};
        float mPadding0{};
        glm::vec3 mBiTangent{};
        float mPadding1{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mPosition, mUVx, mNormal, mUVy, mTangent, mPadding0, mBiTangent, mPadding1);
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
        int mBaseTextureIndex{};
        int mRoughnessTextureIndex{};
        int mOcclusionTextureIndex{};
        int mEmissiveTextureIndex{};
        glm::vec3 mEmissiveFactor{};
        float mMetallicFactor{};
        float mRoughnessFactor{};
        float mAO{};
        float mAlphaCutoff{};
        float mIOR{};
        int mNormalTextureIndex{};
        int padding0{};
        int padding1{};
        int padding2{};

        // bool mDoubleSided{};             // 1 byte
        // glm::vec3 mPadding0{};              // Padding to align to 16 bytes

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mBaseColorFactor,
                    mBaseTextureIndex,
                    mRoughnessTextureIndex,
                    mOcclusionTextureIndex,
                    mEmissiveTextureIndex,
                    mEmissiveFactor,
                    mMetallicFactor,
                    mRoughnessFactor,
                    mAO,
                    mAlphaCutoff,
                    mIOR,
                    mNormalTextureIndex);
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
        int mImageIndex{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mImageIndex);
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
        std::vector<std::string> mImageURIs{};

        std::vector<Vertex> mVertices{};
        std::vector<uint32_t> mIndices{};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(mRootNodes, mNodes, mMeshes, mMaterials, mTextures, mImageURIs, mVertices, mIndices);
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