#pragma once

#include <print>

#define EERROR(fmt, ...) \
        std::println(stderr, "[ERROR] {}:{}:{} \n\t" fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

#define EINFO(fmt, ...) \
        std::println("[INFO ] {}:{}:{} \n\t" fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);