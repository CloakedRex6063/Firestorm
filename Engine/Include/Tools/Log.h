#pragma once
#include "print"

namespace FS
{
    class Log
    {
    public:
        Log();
        template <typename... Args> static void Info(const std::format_string<Args...>& fmt, Args&&... args);
        template <typename... Args> static void Warn(const std::format_string<Args...>& fmt, Args&&... args);
        template <typename... Args> static void Error(const std::format_string<Args...>& fmt, Args&&... args);
        template <typename... Args> static void Critical(const std::format_string<Args...>& fmt, Args&&... args);

    private:
        static constexpr auto lightRed = "\033[91m";
        static constexpr auto yellow = "\033[33m";
        static constexpr auto green = "\033[32m";
        static constexpr auto red = "\033[31m";
        static constexpr auto reset = "\033[0m";
    };

    template <typename... Args> void Log::Info(const std::format_string<Args...>& fmt, Args&&... args)
    {
        std::print("{}[Info] {}{}\n", green, std::format(fmt, std::forward<Args>(args)...), reset);
    }
    template <typename... Args> void Log::Warn(const std::format_string<Args...>& fmt, Args&&... args)
    {
        std::print("{}[Warning] {}{}\n", yellow, std::format(fmt, std::forward<Args>(args)...), reset);
    }
    template <typename... Args> void Log::Error(const std::format_string<Args...>& fmt, Args&&... args)
    {
        std::print("{}[Error] {}{}\n", red, std::format(fmt, std::forward<Args>(args)...), reset);
    }
    template <typename... Args> void Log::Critical(const std::format_string<Args...>& fmt, Args&&... args)
    {
        std::print("{}[Critical] {}{}\n", lightRed, std::format(fmt, std::forward<Args>(args)...), reset);
    }
}