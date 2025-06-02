#pragma once
#include <random>

namespace FS
{
    inline void ThrowError(const std::string_view error)
    {
        MessageBox(nullptr, error.data(), "Error", MB_ICONERROR | MB_OK);
        exit(-1);
    }

    template <typename... Args>
    constexpr std::string FormatString(const std::format_string<Args...>& fmt, Args&&... args)
    {
        return std::format(fmt, std::forward<Args>(args)...);
    };

    inline std::string ReplaceString(const std::string& subject, const std::string& search, const std::string& replace)
    {
        std::string result(subject);
        size_t pos = 0;

        while ((pos = subject.find(search, pos)) != std::string::npos)
        {
            result.replace(pos, search.length(), replace);
            pos += search.length();
        }

        return result;
    }

    inline std::mt19937 g_random_engine;

    template <typename T>
    T RandomNumber(T min, T max)
    {
        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(g_random_engine);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(g_random_engine);
        }
        else
        {
            static_assert(false, "Unsupported type for RandomNumber");
        }
        return 0;
    }

    inline GUID CreateGuid()
    {
        GUID guid;
        HRESULT result = E_FAIL;
        for (int i = 0; i < 10 && FAILED(result); ++i)
        {
            result = CoCreateGuid(&guid);
        }

        if (FAILED(result))
        {
            ThrowError("Failed to create GUID");
        }
        return guid;
    }

    struct TypeHash
    {
        uint64_t Value;

        bool operator==(const TypeHash& other) const
        {
            return Value == other.Value;
        }
    };

    template <typename T>
    TypeHash Hash()
    {
        return {std::hash<std::string>{}(std::string(typeid(T).name()))};
    }
}

template <>
struct std::hash<FS::TypeHash>
{
    size_t operator()(const FS::TypeHash& th) const noexcept
    {
        return std::hash<uint64_t>{}(th.Value);
    }
};
