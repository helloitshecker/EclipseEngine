#pragma once

#include <print>
#include <string_view>
#include <format>

constexpr static std::string_view RedBold       = "\033[1;31m";
constexpr static std::string_view YellowBold    = "\033[1;33m";
constexpr static std::string_view CyanBold      = "\033[1;36m";
constexpr static std::string_view Reset         = "\033[0m";

namespace Eclipse {
      template<typename... Args>
      void LogError(std::format_string<Args...> fmt, Args&&... args) {
            std::print(stderr, "{}[ERROR]{} ", RedBold, Reset);
            std::print(stderr, fmt, std::forward<Args>(args)...);
            std::println();
      }

      template<typename... Args>
      void LogWarn(std::format_string<Args...> fmt, Args&&... args) {
            std::print("{}[WARN]{}  ", YellowBold, Reset);
            std::print(fmt, std::forward<Args>(args)...);
            std::println();
      }

      template<typename... Args>
      void LogInfo(std::format_string<Args...> fmt, Args&&... args) {
            std::print("{}[INFO]{}  ", CyanBold, Reset);
            std::print(fmt, std::forward<Args>(args)...);
            std::println();
      }
};
