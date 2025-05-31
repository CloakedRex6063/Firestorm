#pragma once

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
}
