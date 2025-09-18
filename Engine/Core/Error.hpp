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

namespace Eclipse {
    
    enum class ErrorType {
        None,
        Unknown,
        Window_Unreachable,
        Window_NotOpening,
        Filesystem_InvalidPath,
        Filesystem_FileNotFound
    };

    struct Error {
        ErrorType type;
        std::string message;

        Error() : type(ErrorType::None), message() {}
        Error(ErrorType error_type) : type(error_type), message() {}
        Error(const std::string& error_message) : type(ErrorType::Unknown), message(error_message) {}
        Error(std::string&& error_message) : type(ErrorType::Unknown), message(std::move(error_message)) {}
        Error(ErrorType error_type, const std::string& error_message) : type(error_type), message(error_message) {}
        Error(const std::string& error_message, ErrorType error_type) : type(error_type), message(error_message) {}
        Error(ErrorType error_type, std::string&& error_message) : type(error_type), message(std::move(error_message)) {}
        Error(std::string&& error_message, ErrorType error_type) : type(error_type), message(std::move(error_message)) {}

        explicit operator bool() const {
            return type != ErrorType::None;
        }
    };
}