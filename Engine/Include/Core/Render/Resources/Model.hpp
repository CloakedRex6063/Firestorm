#pragma once

namespace FS
{
    struct Vertex
    {
        glm::vec3 mPosition{};
        float mUVx{};
        glm::vec3 mNormal{};
        float mUVy{};
        glm::vec4 mColor{};
    };
    
    struct Scene
    {
        std::string mName{};
        std::vector<size_t> mNodes{};
    };

    struct Node
    {
        std::string mName{};
        glm::mat4 mTransform{};
        int mMeshIndex{};
        std::vector<size_t> mChildren{};
    };

    struct Mesh
    {
        std::vector<Vertex> mVertices{};
        std::vector<uint32_t> mIndices{};
        glm::vec4 mBaseColor{};
        int mTextureIndex{};
    };

    struct Texture
    {
        std::string mName{};
        int mWidth{};
        int mHeight{};
        std::vector<uint8_t> mPixels{};
    };

    struct Model
    {
        std::string mName{};
        Scene mScene{};
        std::vector<Node> mNodes{};
        std::vector<Mesh> mMeshes{};
        std::vector<Texture> mTextures{};
    };
}