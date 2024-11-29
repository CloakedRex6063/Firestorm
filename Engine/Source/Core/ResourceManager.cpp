#include "Core/ResourceManager.h"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/glm_element_traits.hpp"
#include "Tools/Serializer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace FS
{
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
        constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
                                     fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages |
                                     fastgltf::Options::GenerateMeshIndices;

        constexpr auto extensions =
            fastgltf::Extensions::KHR_materials_transmission | fastgltf::Extensions::KHR_materials_volume;

        Log::Info("Loading Gltf from {}", modelPath.string());

        auto gltfFile = fastgltf::MappedGltfFile::FromPath(modelPath);
        if (!gltfFile)
        {
            Log::Error("Error while loading gltf file {}, {}", modelPath.string(), fastgltf::getErrorMessage(gltfFile.error()));
            return std::nullopt;
        }
        fastgltf::Parser parser(extensions);
        auto expectedAsset = parser.loadGltf(gltfFile.get(), modelPath.parent_path(), gltfOptions);
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
        model.mSamplers.reserve(asset.samplers.size());
        model.mImages.reserve(asset.images.size());
        model.mLights.reserve(asset.lights.size());

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
        LoadGltfSamplers(model, asset);
        LoadGltfImages(model, asset);
        LoadGltfLights(model, asset);

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

            auto& positionAccessor = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];
            const auto oldVerticesSize = static_cast<uint32_t>(model.mVertices.size());
            model.mVertices.resize(oldVerticesSize + positionAccessor.count);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
                                                          positionAccessor,
                                                          [&](const glm::vec3& position, const size_t index)
                                                          {
                                                              model.mVertices[oldVerticesSize + index].mPosition = position;
                                                          });

            if (const auto it = primitive.findAttribute("NORMAL"); it != primitive.attributes.end())
            {
                auto& normalAccessor = asset.accessors[it->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
                                                              normalAccessor,
                                                              [&](const glm::vec3& normal, const size_t index)
                                                              {
                                                                  model.mVertices[oldVerticesSize + index].mNormal = normal;
                                                              });
            }
            if (const auto it = primitive.findAttribute("TEXCOORD_0"); it != primitive.attributes.end())
            {
                auto& uvAccessor = asset.accessors[it->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec2>(asset,
                                                              uvAccessor,
                                                              [&](const glm::vec2& uv, const size_t index)
                                                              {
                                                                  model.mVertices[oldVerticesSize + index].mUVx = uv.x;
                                                                  model.mVertices[oldVerticesSize + index].mUVy = uv.y;
                                                              });
            }
            if (const auto it = primitive.findAttribute("COLOR_0"); it != primitive.attributes.end())
            {
                switch (auto& colorAccessor = asset.accessors[it->accessorIndex]; colorAccessor.type)
                {
                    case fastgltf::AccessorType::Vec3:
                        fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
                                                                      colorAccessor,
                                                                      [&](const glm::vec3& color, const size_t index)
                                                                      {
                                                                          model.mVertices[oldVerticesSize + index].mColor =
                                                                              glm::vec4(color, 1);
                                                                      });
                        break;
                    case fastgltf::AccessorType::Vec4:
                        fastgltf::iterateAccessorWithIndex<glm::vec4>(asset,
                                                                      colorAccessor,
                                                                      [&](const glm::vec4& color, const size_t index)
                                                                      {
                                                                          model.mVertices[oldVerticesSize + index].mColor =
                                                                              glm::vec4(color);
                                                                      });
                        break;
                    default:
                        break;
                }
            }
            else
            {
                std::span currentVertices(model.mVertices.begin() + oldVerticesSize, model.mVertices.end());
                std::ranges::for_each(currentVertices,
                                      [](Vertex& vertex)
                                      {
                                          vertex.mColor = glm::vec4(1.0f);
                                      });
            }

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
                const auto texIndex = baseTexture.value().textureIndex;
                baseColorTextureIndex = static_cast<int>(asset.textures[texIndex].imageIndex.value());
            }

            int metallicRoughnessTextureIndex = -1;
            if (metallicRoughnessTexture.has_value())
            {
                const auto texIndex = metallicRoughnessTexture.value().textureIndex;
                metallicRoughnessTextureIndex = static_cast<int>(asset.textures[texIndex].imageIndex.value());
            }

            int occlusionTextureIndex= -1;
            float ao = 1.f;
            if(material.occlusionTexture.has_value())
            {
                occlusionTextureIndex = material.occlusionTexture.value().textureIndex;
            }

            int emissiveTextureIndex = -1;
            glm::vec3 emissiveFactor = glm::vec3(1.0f);
            if (material.emissiveTexture.has_value())
            {
                const auto texIndex = material.emissiveTexture.value().textureIndex;
                emissiveTextureIndex = texIndex;
                emissiveFactor = glm::make_vec3(material.emissiveFactor.data());
            }

            AlphaMode alphaMode =
                material.alphaMode == fastgltf::AlphaMode::Opaque ? AlphaMode::eOpaque : AlphaMode::eTransparent;

            model.mMaterials.emplace_back(baseColorFactor,
                                          metallicFactor,
                                          roughnessFactor,
                                          baseColorTextureIndex,
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
    void ResourceManager::LoadGltfSamplers(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& [magFilter, minFilter, wrapS, wrapT, name] : asset.samplers)
        {
            auto minSampleFilter = minFilter ? Tools::GetTextureFilter(minFilter.value()) : TextureFilter::eLinear;
            auto magSampleFilter = magFilter ? Tools::GetTextureFilter(magFilter.value()) : TextureFilter::eLinear;
            auto sampleWrapS = Tools::GetTextureWrap(wrapS);
            auto sampleWrapT = Tools::GetTextureWrap(wrapT);

            model.mSamplers.emplace_back(minSampleFilter, magSampleFilter, sampleWrapS, sampleWrapT);
        }
    }
    void ResourceManager::LoadGltfImages(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& [source, name] : asset.images)
        {
            int width = 0, height = 0, channels = 0;
            std::vector<uint8_t> pixels;

            std::visit(
                fastgltf::visitor{
                    [](auto& arg) {},
                    [&](const fastgltf::sources::URI& filePath)
                    {
                        assert(filePath.fileByteOffset == 0);  // We don't support offsets with stbi.
                        assert(filePath.uri.isLocalPath());    // We're only capable of loading
                        // local files.

                        const std::string path(filePath.uri.path().begin(),
                                               filePath.uri.path().end());  // Thanks C++.
                        if (unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4))
                        {
                            pixels.resize(width * height * 4);
                            std::memcpy(pixels.data(), data, width * height * 4);
                            stbi_image_free(data);
                        }
                    },
                    [&](const fastgltf::sources::Vector& vector)
                    {
                        unsigned char* data = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(vector.bytes.data()),
                                                                    static_cast<int>(vector.bytes.size()),
                                                                    &width,
                                                                    &height,
                                                                    &channels,
                                                                    4);
                        if (data)
                        {
                            pixels.resize(width * height * 4);
                            std::memcpy(pixels.data(), data, width * height * 4);
                            stbi_image_free(data);
                        }
                    },
                    [&](const fastgltf::sources::Array& vector)
                    {
                        unsigned char* data = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(vector.bytes.data()),
                                                                    static_cast<int>(vector.bytes.size()),
                                                                    &width,
                                                                    &height,
                                                                    &channels,
                                                                    4);
                        if (data)
                        {
                            pixels.resize(width * height * 4);
                            std::memcpy(pixels.data(), data, width * height * 4);
                            stbi_image_free(data);
                        }
                    },
                    [&](const fastgltf::sources::BufferView& view)
                    {
                        auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                        auto& buffer = asset.buffers[bufferView.bufferIndex];

                        std::visit(
                            fastgltf::visitor{
                                // We only care about VectorWithMime here, because we
                                // specify LoadExternalBuffers, meaning all buffers
                                // are already loaded into a vector.
                                [](auto& arg) {},
                                [&](const fastgltf::sources::Vector& vector)
                                {
                                    unsigned char* data = stbi_load_from_memory(
                                        reinterpret_cast<const uint8_t*>(vector.bytes.data() + bufferView.byteOffset),
                                        static_cast<int>(bufferView.byteLength),
                                        &width,
                                        &height,
                                        &channels,
                                        4);
                                    if (data)
                                    {
                                        pixels.resize(width * height * 4);
                                        std::memcpy(pixels.data(), data, width * height * 4);
                                        stbi_image_free(data);
                                    }
                                },
                                [&](const fastgltf::sources::Array& vector)
                                {
                                    unsigned char* data = stbi_load_from_memory(
                                        reinterpret_cast<const uint8_t*>(vector.bytes.data() + bufferView.byteOffset),
                                        static_cast<int>(bufferView.byteLength),
                                        &width,
                                        &height,
                                        &channels,
                                        4);
                                    if (data)
                                    {
                                        pixels.resize(width * height * 4);
                                        std::memcpy(pixels.data(), data, width * height * 4);
                                        stbi_image_free(data);
                                    }
                                }},

                            buffer.data);
                    },
                },
                source);
            model.mImages.emplace_back(width, height, pixels);
        }
    }
    void ResourceManager::LoadGltfLights(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& [type, color, intensity, range, innerConeAngle, outerConeAngle, name] : asset.lights)
        {
            auto lightType = Tools::GetLightType(type);
            auto lightColor = glm::make_vec3(color.data());
            float lightRange = range ? range.value() : 1.0f;
            float lightInnerConeAngle = innerConeAngle ? innerConeAngle.value() : 0.0f;
            float lightOuterConeAngle = outerConeAngle ? outerConeAngle.value() : 0.0f;

            model.mLights.emplace_back(lightType, lightColor, intensity, lightRange, lightInnerConeAngle, lightOuterConeAngle);
        }
    }
};  // namespace FS