#pragma once

#include <stdio.h>
#include <stdlib.h>

// Logging macros
#define EERROR(msg, ...)  fprintf(stderr, "[ERROR] %s:%d::%s() \n\t" msg "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#ifdef NDEBUG
#define EWARN(msg, ...)
#define EINFO(msg, ...)
#define EDEBUG(msg, ...)
#else
#define EWARN(msg, ...)   fprintf(stdout, "[WARN ] %s:%d::%s() \n\t" msg "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define EINFO(msg, ...)   fprintf(stdout, "[INFO ] %s:%d::%s() \n\t" msg "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define EDEBUG(msg, ...)  fprintf(stdout, "[DEBUG] %s:%d::%s() \n\t" msg "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif