#pragma once

#include <print>
#include <cstdlib>

#ifdef _WIN32
    #define DEBUG_BREAK() __debugbreak();
#else
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifndef NDEBUG
    #define ECL_ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            std::println("ASSERTION FAILED: {} {} \nCondition: {}", __FILE__, __LINE__, #cond); \
            std::println("{}", __VA_ARGS__); \
            DEBUG_BREAK(); \
            std::abort(); \
        } \
    } while (0)
#else
    #define ECL_ASSERT(cond, ...) ((void)0)
#endif
