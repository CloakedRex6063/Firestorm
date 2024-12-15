#include "Core/ResourceManager.h"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/glm_element_traits.hpp"
#include "Tools/Serializer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace FS
{
    ResourceManager::ResourceManager()
    {
        constexpr auto extensions = fastgltf::Extensions::KHR_materials_transmission |
                                    fastgltf::Extensions::KHR_materials_volume;
        mParser = fastgltf::Parser(extensions);
    }

    std::optional<ResourceHandle> ResourceManager::LoadModel(const std::string& modelPath)
    {
        auto resource = LoadModelBinary(modelPath);
        if (resource.has_value())
        {
            return resource;
        }
        const std::filesystem::path resourcePath = modelPath;
        if (resourcePath.extension() == ".glb" || resourcePath.extension() == ".gltf")
        {
            resource = LoadGltf(resourcePath);
        }

        return resource;
    }

    std::optional<ResourceHandle> ResourceManager::LoadModelBinary(const std::string& modelPath)
    {
        const auto model = Serializer::Deserialize<Model>("model.bin");
        if (model.has_value())
        {
            mUploadQueue[modelPath] = model.value();
            Log::Error("Successfully loaded Model %s", modelPath.c_str());
            return ResourceHandle();
        }

        return std::nullopt;
    }

    std::optional<ResourceHandle> ResourceManager::LoadGltf(const std::filesystem::path& modelPath)
    {
        constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers |
                                     fastgltf::Options::GenerateMeshIndices;

        Log::Info("Loading Gltf from {}", modelPath.string());

        auto gltfFile = fastgltf::MappedGltfFile::FromPath(modelPath);
        if (!gltfFile)
        {
            Log::Error("Error while loading gltf file {}, {}", modelPath.string(), fastgltf::getErrorMessage(gltfFile.error()));
            return std::nullopt;
        }
        auto expectedAsset = mParser.loadGltf(gltfFile.get(), modelPath.parent_path(), gltfOptions);
        if (!expectedAsset)
        {
            // Log the error
            Log::Error("Error while parsing gltf file {}, {}",
                       modelPath.string(),
                       fastgltf::getErrorMessage(expectedAsset.error()));
            return std::nullopt;
        }

        const auto asset = std::move(expectedAsset.get());
        Log::Info("Node count : {}", asset.nodes.size());
        Log::Info("Mesh count : {}", asset.meshes.size());
        Log::Info("Material count : {}", asset.materials.size());
        Log::Info("Texture count : {}", asset.textures.size());
        Log::Info("Sampler count : {}", asset.samplers.size());
        Log::Info("Image count : {}", asset.images.size());
        Log::Info("Light count : {}", asset.lights.size());

        auto model = Model();
        auto assetNodes = asset.scenes[0].nodeIndices;
        model.mRootNodes.reserve(assetNodes.size());
        model.mNodes.reserve(asset.nodes.size());
        model.mMeshes.reserve(asset.meshes.size());
        model.mMaterials.reserve(asset.materials.size());
        model.mImageURIs.reserve(asset.images.size());

        std::ranges::transform(assetNodes,
                               std::back_inserter(model.mRootNodes),
                               [](const auto& node)
                               {
                                   return static_cast<uint32_t>(node);
                               });
        LoadGltfNodes(model, asset);
        LoadGltfMeshes(model, asset);
        LoadGltfMaterials(model, asset);
        LoadGltfImages(model, asset);

        //Serializer::Serialize("model.bin", model);

        mUploadQueue[modelPath.string()] = model;
        return ResourceHandle();
    }

    void ResourceManager::LoadGltfNodes(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& node : asset.nodes)
        {
            auto transform = fastgltf::getTransformMatrix(node);
            const auto glmTransform = glm::make_mat4(transform.data());

            Node newNode{.mTransform = glmTransform,
                         .mMeshIndex = node.meshIndex ? static_cast<int>(node.meshIndex.value()) : -1,
                         .mLightIndex = node.lightIndex ? static_cast<int>(node.lightIndex.value()) : -1};
            std::ranges::transform(node.children,
                                   std::back_inserter(newNode.mChildren),
                                   [](const auto& child)
                                   {
                                       return static_cast<uint32_t>(child);
                                   });
            model.mNodes.emplace_back(std::move(newNode));
        }
    }
    void ResourceManager::LoadGltfMeshes(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& mesh : asset.meshes)
        {
            auto& primitive = mesh.primitives[0];
            auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];

            const auto oldVerticesSize = static_cast<uint32_t>(model.mVertices.size());
            const auto oldIndicesSize = static_cast<uint32_t>(model.mIndices.size());

            model.mIndices.resize(oldIndicesSize + indexAccessor.count);
            switch (indexAccessor.componentType)
            {
                case fastgltf::ComponentType::UnsignedShort:
                {
                    std::vector<uint16_t> shortIndices(indexAccessor.count);
                    fastgltf::copyFromAccessor<uint16_t>(asset, indexAccessor, shortIndices.data());
                    std::ranges::copy(shortIndices, model.mIndices.begin() + oldIndicesSize);
                    break;
                }
                case fastgltf::ComponentType::UnsignedInt:
                {
                    fastgltf::copyFromAccessor<uint32_t>(asset, indexAccessor, model.mIndices.data() + oldIndicesSize);
                    break;
                }
                default:
                    break;
            }

            assert(primitive.findAttribute("POSITION"));
            auto& positionAccessor = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];

            model.mVertices.resize(oldVerticesSize + positionAccessor.count);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
                                                          positionAccessor,
                                                          [&](const glm::vec3& position, const size_t index)
                                                          {
                                                              model.mVertices[oldVerticesSize + index].mPosition = position;
                                                          });

            assert(primitive.findAttribute("NORMAL")->accessorIndex);
            auto& normalAccessor = asset.accessors[primitive.findAttribute("NORMAL")->accessorIndex];
            fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
                                                          normalAccessor,
                                                          [&](const glm::vec3& normal, const size_t index)
                                                          {
                                                              model.mVertices[oldVerticesSize + index].mNormal = normal;
                                                          });

            assert(primitive.findAttribute("TEXCOORD_0"));
            auto& uvAccessor = asset.accessors[primitive.findAttribute("TEXCOORD_0")->accessorIndex];
            fastgltf::iterateAccessorWithIndex<glm::vec2>(asset,
                                                          uvAccessor,
                                                          [&](const glm::vec2& uv, const size_t index)
                                                          {
                                                              model.mVertices[oldVerticesSize + index].mUVx = uv.x;
                                                              model.mVertices[oldVerticesSize + index].mUVy = uv.y;
                                                          });

            auto materialIndex = primitive.materialIndex ? static_cast<int>(primitive.materialIndex.value()) : -1;
            model.mMeshes.emplace_back(oldVerticesSize,
                                       oldIndicesSize,
                                       static_cast<uint32_t>(indexAccessor.count),
                                       materialIndex);
        }
    }

    // From https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    void ResourceManager::LoadGltfTangents(Model& model)
    {
        auto& vertices = model.mVertices;
        auto& indices = model.mIndices;
        for (int i = 0; i < indices.size(); ++i)
        {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            Vertex& v0 = vertices[i0];
            Vertex& v1 = vertices[i1];
            Vertex& v2 = vertices[i2];

            glm::vec3 p0 = v0.mPosition;
            glm::vec3 p1 = v1.mPosition;
            glm::vec3 p2 = v2.mPosition;

            glm::vec2 uv0 = glm::vec2(v0.mUVx, v0.mUVy);
            glm::vec2 uv1 = glm::vec2(v1.mUVx, v1.mUVy);
            glm::vec2 uv2 = glm::vec2(v2.mUVx, v2.mUVy);

            glm::vec3 edge1 = p1 - p0;
            glm::vec3 edge2 = p2 - p0;

            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float det = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
            if (det != 0.0f)
            {
                float invDet = 1.0f / det;

                glm::vec3 tangent = invDet * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                glm::vec3 bitangent = invDet * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

                v0.mTangent += tangent;
                v1.mTangent += tangent;
                v2.mTangent += tangent;

                v0.mBiTangent += bitangent;
                v1.mBiTangent += bitangent;
                v2.mBiTangent += bitangent;
            }
        }

        for (auto& vertex : model.mVertices)
        {
            vertex.mTangent = glm::normalize(vertex.mTangent);
            vertex.mBiTangent = glm::normalize(vertex.mBiTangent);
        }
    }

    void ResourceManager::LoadGltfMaterials(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& material : asset.materials)
        {
            const auto& [baseFactor, metallicFactor, roughnessFactor, baseTexture, metallicRoughnessTexture] = material.pbrData;
            auto baseColorFactor = glm::make_vec4(baseFactor.data());

            int baseColorImageIndex = -1;
            if (baseTexture.has_value())
            {
                const auto baseColorTextureIndex = static_cast<int>(baseTexture.value().textureIndex);
                baseColorImageIndex = static_cast<int>(asset.textures[baseColorTextureIndex].imageIndex.value());
            }

            int metallicRoughnessImageIndex = -1;
            if (metallicRoughnessTexture.has_value())
            {
                const auto metallicRoughnessTextureIndex = static_cast<int>(metallicRoughnessTexture.value().textureIndex);
                metallicRoughnessImageIndex =
                    static_cast<int>(asset.textures[metallicRoughnessTextureIndex].imageIndex.value());
            }

            int occlusionImageIndex = -1;
            float ao = 1.f;
            if (material.occlusionTexture.has_value())
            {
                const auto occlusionTextureIndex = static_cast<int>(material.occlusionTexture.value().textureIndex);
                occlusionImageIndex = static_cast<int>(asset.textures[occlusionTextureIndex].imageIndex.value());
                ao = material.occlusionTexture.value().strength;
            }

            int emissiveImageIndex = -1;
            auto emissiveFactor = glm::vec3(1.0f);
            if (material.emissiveTexture.has_value())
            {
                const auto emissiveTextureIndex = static_cast<int>(material.emissiveTexture.value().textureIndex);
                emissiveImageIndex = static_cast<int>(asset.textures[emissiveTextureIndex].imageIndex.value());
                emissiveFactor = glm::make_vec3(material.emissiveFactor.data());
            }

            int normalImageIndex = -1;
            if (material.normalTexture.has_value())
            {
                const auto normalTextureIndex = static_cast<int>(material.normalTexture.value().textureIndex);
                normalImageIndex = static_cast<int>(asset.textures[normalTextureIndex].imageIndex.value());
            }

            AlphaMode alphaMode = material.alphaMode == fastgltf::AlphaMode::Opaque ? AlphaMode::eOpaque :
                                                                                      AlphaMode::eTransparent;

            model.mMaterials.emplace_back(baseColorFactor,
                                          baseColorImageIndex,
                                          metallicRoughnessImageIndex,
                                          occlusionImageIndex,
                                          emissiveImageIndex,
                                          emissiveFactor,
                                          metallicFactor,
                                          roughnessFactor,
                                          ao,
                                          material.alphaCutoff,
                                          material.ior,
                                          normalImageIndex);
        }
    }

    void ResourceManager::LoadGltfImages(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& [data, name] : asset.images)
        {
            assert(std::holds_alternative<fastgltf::sources::URI>(data));
            model.mImageURIs.emplace_back(std::get<fastgltf::sources::URI>(data).uri.string());
        }
    }
};  // namespace FS