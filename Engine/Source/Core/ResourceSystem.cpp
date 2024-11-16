#include "Core/ResourceSystem.h"
#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/tools.hpp"
#include "stb_image.h"

namespace FS
{
    ResourceSystem::ResourceSystem() { mParser = std::make_unique<fastgltf::Parser>(); }

    std::optional<ResourceHandle> ResourceSystem::LoadModel(const std::string& filePath)
    {
        const auto resourcePath = std::filesystem::path(filePath);

        std::optional<Model> loadedModel;
        if (resourcePath.extension() == ".glb" || resourcePath.extension() == ".gltf")
        {
            loadedModel = LoadGltf(resourcePath);
        }

        if (loadedModel.has_value())
        {
            mModelsToUpload.emplace(resourcePath.string(), loadedModel.value());
            auto resourceHandle = ResourceHandle(std::make_shared<std::string>(resourcePath.string()));
            mModels.emplace(resourceHandle);
            return resourceHandle;
        }

        return std::nullopt;
    }

    std::unordered_map<std::string, Model> ResourceSystem::GetModelsToUpload()
    {
        std::unordered_map<std::string, Model> newModels = std::move(mModelsToUpload);
        mModelsToUpload.clear();
        return newModels;
    }

    std::unordered_set<std::string> ResourceSystem::GetModelsToRelease()
    {
        std::unordered_set<std::string> modelsToRelease;
        for (const auto& handle : mModels)
        {
            if (handle.mResourceReference.use_count() == 1)
            {
                Log::Info("Unloading mesh : {}", *handle.mResourceReference);
                modelsToRelease.emplace(*handle.mResourceReference);
                mModels.erase(handle);
            }
        }

        return modelsToRelease;
    }

    std::optional<Model> ResourceSystem::LoadGltf(const std::filesystem::path& path) const
    {
        constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
                                     fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages |
                                     fastgltf::Options::GenerateMeshIndices;

        Log::Info("Loading Gltf from {}", path.string());

        auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
        if (!gltfFile)
        {
            Log::Error("Error while loading gltf file {}, {}", path.string(),
                       fastgltf::getErrorMessage(gltfFile.error()));
            return std::nullopt;
        }
        auto expectedAsset = mParser->loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
        if (!expectedAsset)
        {
            // Log the error
            Log::Error("Error while parsing gltf file {}, {}", path.string(),
                       fastgltf::getErrorMessage(expectedAsset.error()));
            return std::nullopt;
        }

        auto asset = std::move(expectedAsset.get());
        Log::Info("Node count : {}", asset.nodes.size());
        Log::Info("Mesh count : {}", asset.meshes.size());
        Log::Info("Texture count : {}", asset.textures.size());
        auto model = Model();
        model.mNodes.reserve(asset.nodes.size());
        model.mMeshes.reserve(asset.meshes.size());
        model.mTextures.reserve(asset.textures.size());
        LoadGltfMeshes(model, asset);
        LoadGltfTextures(model, asset);
        LoadGltfNodes(model, asset);
        model.mName = path.stem().string();

        auto assetNodes = asset.scenes[0].nodeIndices;
        model.mScene.mNodes.assign(assetNodes.begin(), assetNodes.end());
        return model;
    }

    void ResourceSystem::LoadGltfMeshes(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& mesh : asset.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                std::vector<uint32_t> indices;
                std::vector<uint16_t> shortIndices;
                if (primitive.indicesAccessor.has_value())
                {
                    auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
                    indices.resize(indexAccessor.count);
                    switch (indexAccessor.componentType)
                    {
                    case fastgltf::ComponentType::UnsignedShort:
                        shortIndices.resize(indexAccessor.count);
                        fastgltf::copyFromAccessor<uint16_t>(asset, indexAccessor, shortIndices.data());
                        std::ranges::transform(shortIndices, indices.begin(),
                                               [](const uint16_t value)
                                               {
                                                   return static_cast<uint32_t>(value);
                                               });
                        break;
                    case fastgltf::ComponentType::UnsignedInt:
                        fastgltf::copyFromAccessor<uint32_t>(asset, indexAccessor, indices.data());
                    default:;
                    }
                }
                std::vector<Vertex> vertices;
                if (const auto it = primitive.findAttribute("POSITION"); it != primitive.attributes.end())
                {
                    auto& positionAccessor = asset.accessors[it->accessorIndex];
                    vertices.resize(positionAccessor.count);
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, positionAccessor,
                                                                  [&](const glm::vec3& position, const size_t index)
                                                                  {
                                                                      vertices[index].mPosition = position;
                                                                  });
                }
                if (const auto it = primitive.findAttribute("NORMAL"); it != primitive.attributes.end())
                {
                    auto& normalAccessor = asset.accessors[it->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, normalAccessor,
                                                                  [&](const glm::vec3& normal, const size_t index)
                                                                  {
                                                                      vertices[index].mNormal = normal;
                                                                  });
                }
                if (const auto it = primitive.findAttribute("TEXCOORD_0"); it != primitive.attributes.end())
                {
                    auto& uvAccessor = asset.accessors[it->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(asset, uvAccessor,
                                                                  [&](const glm::vec2& uv, const size_t index)
                                                                  {
                                                                      vertices[index].mUVx = uv.x;
                                                                      vertices[index].mUVy = uv.y;
                                                                  });
                }
                if (const auto it = primitive.findAttribute("COLOR_0"); it != primitive.attributes.end())
                {
                    auto& colorAccessor = asset.accessors[it->accessorIndex];
                    switch (colorAccessor.type)
                    {
                    case fastgltf::AccessorType::Vec3:
                        fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, colorAccessor,
                                                                      [&](const glm::vec3& color, const size_t index)
                                                                      {
                                                                          vertices[index].mColor = glm::vec4(color, 1);
                                                                      });
                        break;
                    case fastgltf::AccessorType::Vec4:
                        fastgltf::iterateAccessorWithIndex<glm::vec4>(asset, colorAccessor,
                                                                      [&](const glm::vec4& color, const size_t index)
                                                                      {
                                                                          vertices[index].mColor = color;
                                                                      });
                        break;
                    default:
                        break;
                    }
                }

                int textureIndex = -1;
                auto baseColor = glm::vec4(1.0);
                if (primitive.materialIndex.has_value())
                {
                    const auto& material = asset.materials[primitive.materialIndex.value()];
                    const auto& [baseColorFactor, metallicFactor, roughnessFactor, baseColorTexture,
                                 metallicRoughnessTexture] = material.pbrData;
                    if (baseColorTexture.has_value())
                    {
                        textureIndex = static_cast<int>(baseColorTexture.value().textureIndex);
                        baseColor = glm::make_vec4(baseColorFactor.data());
                    }
                }
                model.mMeshes.emplace_back(std::move(vertices), std::move(indices), baseColor, textureIndex);
            }
        }
    }

    void ResourceSystem::LoadGltfTextures(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& texture : asset.textures)
        {
            const auto& [source, name] = asset.images[texture.imageIndex.value()];
            int width = 0, height = 0, channels = 0;
            std::vector<uint8_t> pixels;

            std::visit(
                fastgltf::visitor{
                    [](auto& arg)
                    {
                    },
                    [&](const fastgltf::sources::URI& filePath)
                    {
                        assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                        assert(filePath.uri.isLocalPath());   // We're only capable of loading
                                                              // local files.

                        const std::string path(filePath.uri.path().begin(),
                                               filePath.uri.path().end()); // Thanks C++.
                        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
                        if (data)
                        {
                            pixels.resize(width * height * 4);
                            std::memcpy(pixels.data(), data, width * height * 4);
                            stbi_image_free(data);
                        }
                    },
                    [&](const fastgltf::sources::Vector& vector)
                    {
                        unsigned char* data =
                            stbi_load_from_memory(reinterpret_cast<const uint8_t*>(vector.bytes.data()),
                                                  static_cast<int>(vector.bytes.size()), &width, &height, &channels, 4);
                        if (data)
                        {
                            pixels.resize(width * height * 4);
                            std::memcpy(pixels.data(), data, width * height * 4);
                            stbi_image_free(data);
                        }
                    },
                    [&](const fastgltf::sources::Array& vector)
                    {
                        unsigned char* data =
                            stbi_load_from_memory(reinterpret_cast<const uint8_t*>(vector.bytes.data()),
                                                  static_cast<int>(vector.bytes.size()), &width, &height, &channels, 4);
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
                                [](auto& arg)
                                {
                                },
                                [&](const fastgltf::sources::Vector& vector)
                                {
                                    unsigned char* data = stbi_load_from_memory(
                                        reinterpret_cast<const uint8_t*>(vector.bytes.data() + bufferView.byteOffset),
                                        static_cast<int>(bufferView.byteLength), &width, &height, &channels, 4);
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
                                        static_cast<int>(bufferView.byteLength), &width, &height, &channels, 4);
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
            model.mTextures.emplace_back(name.c_str(), width, height, pixels);
        }
    }

    void ResourceSystem::LoadGltfNodes(Model& model, const fastgltf::Asset& asset)
    {
        for (const auto& node : asset.nodes)
        {
            auto transform = fastgltf::getTransformMatrix(node);
            auto glmTransform = glm::make_mat4(transform.data());
            std::vector<size_t> children;
            children.assign(node.children.begin(), node.children.end());
            model.mNodes.emplace_back(node.name.c_str(), glmTransform, static_cast<int>(node.meshIndex.value()),
                                      children);
        }
    }
} // namespace FS