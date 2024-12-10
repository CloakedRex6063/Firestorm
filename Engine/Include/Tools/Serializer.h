#pragma once
#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

namespace FS
{
    class Serializer
    {
    public:
        template <typename T>
        static void Serialize(const std::string& path, const T& data);

        template <typename T>
        static std::optional<T> Deserialize(const std::string& path);
    };
    
    template <typename T>
    void Serializer::Serialize(const std::string& path, const T& data)
    {
        std::ofstream os(path, std::ios::binary);
        if (!os.is_open())
        {
            Log::Error("Failed to open file for writing: {}", path);
            return;
        }

        try
        {
            cereal::BinaryOutputArchive output(os);
            output(data);
            os.flush(); // Ensure data is written to disk
        }
        catch (const cereal::Exception& e)
        {
            Log::Error("Cereal serialization error: {}", e.what());
        }
        catch (const std::exception& e)
        {
            Log::Error("General serialization error: {}", e.what());
        }
        catch (...)
        {
            Log::Error("Unknown serialization error");
        }
    }

    template <typename T>
    std::optional<T> Serializer::Deserialize(const std::string& path)
    {
        std::ifstream is(path, std::ios::binary);
        if (!is.is_open())
        {
            Log::Error("Failed to open file for reading: {}", path);
            return std::nullopt;
        }

        // Optional file size check (debugging)
        is.seekg(0, std::ios::end);
        std::streamsize size = is.tellg();
        if (size <= 0)
        {
            Log::Error("File is empty or invalid: {}", path);
            return std::nullopt;
        }
        is.seekg(0, std::ios::beg); // Reset position

        T data;
        try
        {
            cereal::BinaryInputArchive input(is);
            input(data);
        }
        catch (const cereal::Exception& e)
        {
            Log::Error("Cereal deserialization error: {}", e.what());
            return std::nullopt;
        }
        catch (const std::exception& e)
        {
            Log::Error("General deserialization error: {}", e.what());
            return std::nullopt;
        }
        catch (...)
        {
            Log::Error("Unknown deserialization error");
            return std::nullopt;
        }

        return data;
    }

    // template <typename T>
    // T Serializer::Deserialize(const std::string& path)
    // {
    //     auto [data, in, out] = zpp::bits::data_in_out();
    //     auto result = in(data);
    // }
}  // namespace FS

namespace glm
{
    template <class Archive>
    void serialize(Archive& archive, glm::vec2& v)
    {
        archive(v.x, v.y);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::vec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::vec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::ivec2& v)
    {
        archive(v.x, v.y);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::ivec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::ivec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::uvec2& v)
    {
        archive(v.x, v.y);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::uvec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::uvec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dvec2& v)
    {
        archive(v.x, v.y);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dvec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dvec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }

    // glm matrices serialization
    template <class Archive>
    void serialize(Archive& archive, glm::mat2& m)
    {
        archive(m[0], m[1]);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dmat2& m)
    {
        archive(m[0], m[1]);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::mat3& m)
    {
        archive(m[0], m[1], m[2]);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::mat4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dmat4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }

    template <class Archive>
    void serialize(Archive& archive, glm::quat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
    template <class Archive>
    void serialize(Archive& archive, glm::dquat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
}  // namespace glm