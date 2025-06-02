#pragma once
#include "chrono"
#include "Tools.hpp"

namespace FS
{
    inline constexpr auto light_red = "\033[91m";
    inline constexpr auto light_green = "\033[92m";
    inline constexpr auto green = "\033[32m";
    inline constexpr auto yellow = "\033[33m";
    inline constexpr auto red = "\033[31m";
    inline constexpr auto reset = "\033[0m";

    class Log
    {
    public:
        template <typename... Args>
        static constexpr void Debug(const std::format_string<Args...>& fmt, Args&&... args)
        {
            std::print("{}{} [Debug] {}{}\n", light_green, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template <typename... Args>
        static constexpr void Info(const std::format_string<Args...>& fmt, Args&&... args)
        {
            std::print("{}{} [Info] {}{}\n", green, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template <typename... Args>
        static constexpr void Warn(const std::format_string<Args...>& fmt, Args&&... args)
        {
            std::print("{}{} [Warn] {}{}\n", yellow, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template <typename... Args>
        static constexpr void Error(const std::format_string<Args...>& fmt, Args&&... args)
        {
            std::print("{}{} [Error] {}{}\n", light_red, Timestamp(), std::format(fmt, std::forward<Args>(args)...),
                       reset);
        }

        template <typename... Args>
        static constexpr void Critical(const std::format_string<Args...>& fmt, Args&&... args)
        {
            std::print("{}{} [Critical] {}{}\n", red, Timestamp(), std::format(fmt, std::forward<Args>(args)...),
                       reset);
            const auto formattedString = FormatString(fmt, std::forward<Args>(args)...);
            ThrowError(formattedString);
        }

    private:
        static std::string Timestamp()
        {
            std::chrono::zoned_time local_time{std::chrono::current_zone(), std::chrono::system_clock::now()};
            return std::format("[{:%Y-%m-%d %H:%M:%S}]", local_time);
        }
    };
}
