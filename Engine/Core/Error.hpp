#pragma once

#include <print>
#include <string_view>
#include <format>
#include <filesystem>

constexpr static std::string_view RedBold       = "\033[1;31m";
constexpr static std::string_view YellowBold    = "\033[1;33m";
constexpr static std::string_view CyanBold      = "\033[1;36m";
constexpr static std::string_view Reset         = "\033[0m";

namespace fs = std::filesystem;

// Formatting for filesystem paths
template<>
struct std::formatter<fs::path> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const fs::path& p, FormatContext& ctx) const {
        return std::formatter<std::string>::format(p.string(), ctx);
    }
};

namespace Eclipse {
      template<typename... Args>
      void LogError(std::format_string<Args...> fmt, Args&&... args) {
            std::print(stderr, "{}[ERROR]{} ", RedBold, Reset);
            std::println(stderr, fmt, std::forward<Args>(args)...);
      }

      template<typename... Args>
      void LogWarn(std::format_string<Args...> fmt, Args&&... args) {
            std::print("{}[WARN]{}  ", YellowBold, Reset);
            std::println(fmt, std::forward<Args>(args)...);
      }

      template<typename... Args>
      void LogInfo(std::format_string<Args...> fmt, Args&&... args) {
            std::print("{}[INFO]{}  ", CyanBold, Reset);
            std::println(fmt, std::forward<Args>(args)...);
      }
};
