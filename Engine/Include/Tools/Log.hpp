#pragma once
#include "chrono"
#include "Tools.hpp"

namespace FS
{
    static constexpr auto magenta = "\033[35m";
    static constexpr auto green = "\033[32m";
    static constexpr auto yellow = "\033[33m";
    static constexpr auto red = "\033[31m";
    static constexpr auto reset = "\033[0m";

    class Log
    {
    public:
        template <typename ... Args>
        static constexpr void Info(const std::format_string<Args...>& fmt, Args&& ... args)
        {
            std::print("{}{} [Info] {}{}\n", green, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template<typename... Args>
        static constexpr void Warn(const std::format_string<Args...>& fmt, Args&&... args) {
            std::print("{}{} [Warn] {}{}\n", yellow, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template<typename... Args>
        static constexpr void Error(const std::format_string<Args...>& fmt, Args&&... args) {
            std::print("{}{} [Error] {}{}\n", magenta, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
        }

        template<typename... Args>
        static constexpr void Critical(const std::format_string<Args...>& fmt, Args&&... args) {
            std::print("{}{} [Critical] {}{}\n", red, Timestamp(), std::format(fmt, std::forward<Args>(args)...), reset);
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
