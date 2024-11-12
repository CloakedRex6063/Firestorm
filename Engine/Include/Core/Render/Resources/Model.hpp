#pragma once

namespace FS
{
    struct Vertex
    {
        glm::vec3 mPosition;
        float mUVx;
        glm::vec3 mNormal;
        float mUVy;
        glm::vec4 mColor;
    };
    
    struct Scene
    {
        std::string mName;
        std::vector<int> mNodes;
    };

    struct Node
    {
        std::string mName;
        glm::mat4 mTransform;
        int mMeshIndex;
        std::vector<int> mChildren;
    };

    struct Mesh
    {
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        glm::vec4 mColor;
        int mTextureIndex;
    };

    struct Texture
    {
        std::string mName;
        std::vector<uint8_t> mPixels;
    };

    struct Model
    {
        Scene mScene;
        std::vector<Node> mNodes;
        std::vector<Mesh> mMeshes;
        std::vector<Texture> mTextures;
    };
}