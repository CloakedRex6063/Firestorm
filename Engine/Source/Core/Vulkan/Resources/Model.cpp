#include "Core/Render/Vulkan/Resources/Model.h"

namespace FS::VK
{
    Model::Model(const std::shared_ptr<Context>& context,
                 const uint64_t verticesSize,
                 const uint64_t indicesSize,
                 const std::vector<Mesh>& meshes,
                 const std::vector<Node>& nodes,
                 const std::vector<int>& rootNodes,
                 const std::vector<Material>& materials,
                 std::vector<Image> textures)
        : mVertexBuffer(context, BufferType::eVertex, verticesSize),
          mIndexBuffer(context, BufferType::eIndex, indicesSize),
          mRootNodes(std::move(rootNodes)),
          mNodes(std::move(nodes)),
          mMeshes(std::move(meshes)),
          mMaterials(std::move(materials)),
          mTextures(std::move(textures))
    {
    }
}  // namespace FS::VK