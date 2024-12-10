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
        std::optional<ResourceHandle> resource = ResourceManager::LoadModelBinary(modelPath);
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
        auto model = Serializer::Deserialize<Model>("model.bin");

        if (model.has_value())
        {
            mUploadQueue[modelPath] = model.value();
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
        model.mTextures.reserve(asset.textures.size());
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
        LoadGltfTextures(model, asset);
        LoadGltfImages(model, asset);

        Serializer::Serialize("model.bin", model);

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
            const auto oldIndicesSize = static_cast<uint32_t>(model.mIndices.size());
            model.mIndices.reserve(oldIndicesSize + indexAccessor.count);

            std::vector<uint16_t> shortIndices;
            switch (indexAccessor.componentType)
            {
                case fastgltf::ComponentType::UnsignedShort:
                    shortIndices.resize(indexAccessor.count);
                    fastgltf::copyFromAccessor<uint16_t>(asset, indexAccessor, shortIndices.data());
                    std::ranges::copy(shortIndices.begin(), shortIndices.end(), std::back_inserter(model.mIndices));
                    break;
                case fastgltf::ComponentType::UnsignedInt:
                    fastgltf::copyFromAccessor<uint32_t>(asset, indexAccessor, model.mIndices.data() + oldIndicesSize);
                default:
                    break;
            }

            assert(primitive.findAttribute("POSITION"));
            auto& positionAccessor = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];
            const auto oldVerticesSize = static_cast<uint32_t>(model.mVertices.size());
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

    void ResourceManager::LoadGltfMaterials(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& material : asset.materials)
        {
            const auto& [baseFactor, metallicFactor, roughnessFactor, baseTexture, metallicRoughnessTexture] = material.pbrData;
            auto baseColorFactor = glm::make_vec4(baseFactor.data());

            int baseColorTextureIndex = -1;
            if (baseTexture.has_value())
            {
                baseColorTextureIndex = static_cast<int>(baseTexture.value().textureIndex);
            }

            int metallicRoughnessTextureIndex = -1;
            if (metallicRoughnessTexture.has_value())
            {
                metallicRoughnessTextureIndex = static_cast<int>(metallicRoughnessTexture.value().textureIndex);
            }

            int occlusionTextureIndex = -1;
            float ao = 1.f;
            if (material.occlusionTexture.has_value())
            {
                occlusionTextureIndex = static_cast<int>(material.occlusionTexture.value().textureIndex);
                ao = material.occlusionTexture.value().strength;
            }

            int emissiveTextureIndex = -1;
            auto emissiveFactor = glm::vec3(1.0f);
            if (material.emissiveTexture.has_value())
            {
                emissiveTextureIndex = static_cast<int>(material.emissiveTexture.value().textureIndex);
                emissiveFactor = glm::make_vec3(material.emissiveFactor.data());
            }

            int normalTextureIndex = -1;
            if (material.normalTexture.has_value())
            {
                normalTextureIndex = static_cast<int>(material.normalTexture.value().textureIndex);
            }

            AlphaMode alphaMode = material.alphaMode == fastgltf::AlphaMode::Opaque ? AlphaMode::eOpaque :
                                                                                      AlphaMode::eTransparent;

            model.mMaterials.emplace_back(baseColorFactor,
                                          metallicFactor,
                                          roughnessFactor,
                                          baseColorTextureIndex,
                                          normalTextureIndex,
                                          metallicRoughnessTextureIndex,
                                          occlusionTextureIndex,
                                          ao,
                                          emissiveTextureIndex,
                                          emissiveFactor,
                                          alphaMode,
                                          material.doubleSided,
                                          material.alphaCutoff,
                                          material.ior);
        }
    }

    void ResourceManager::LoadGltfTextures(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& texture : asset.textures)
        {
            int samplerIndex = -1;
            if (texture.samplerIndex.has_value())
            {
                samplerIndex = texture.samplerIndex.value();
            }

            int imageIndex = -1;
            if (texture.imageIndex.has_value())
            {
                imageIndex = texture.imageIndex.value();
            }

            model.mTextures.emplace_back(samplerIndex, imageIndex);
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