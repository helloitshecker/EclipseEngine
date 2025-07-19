#pragma once

#include <print>

constexpr std::string_view FORMAT_RESET    = "\e[0m";
constexpr std::string_view FORMAT_RED_B    = "\e[1;31m";
constexpr std::string_view FORMAT_YELLOW_B = "\e[1;33m";
constexpr std::string_view FORMAT_CYAN_B   = "\e[1;36m";

namespace Eclipse::Logger {

    enum class Level {
        INFO,
        WARNING,
        ERROR,
        NONE
    };

    template <typename... Args>
    inline void Log(Level lvl, const std::format_string<Args...> message, Args&&... args) {
        switch (lvl) {
            case Level::INFO : {
                const std::string out = std::format(message, args...);
                std::println("{}INFO:{} {}", FORMAT_CYAN_B, FORMAT_RESET, out);
            } break;
            case Level::WARNING : {
                const std::string out = std::format(message, args...);
                std::println("{}WARNING:{} {}", FORMAT_YELLOW_B, FORMAT_RESET, out);
            } break;
            case Level::ERROR : {
                const std::string out = std::format(message, args...);
                std::println("{}ERROR:{} {}", FORMAT_RED_B, FORMAT_RESET, out);
            } break;
            default: break;
        } // switch
    }

    template <typename... Args>
    inline void Log(const std::format_string<Args...> message, Args&&... args) {
        Log(Level::INFO, message, args...);
    }
}
