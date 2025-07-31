#pragma once

#include <stdio.h>
#include <stdlib.h>

// Logging macros
#define EERROR(msg, ...)  fprintf(stderr, "[ERROR] %s:%d " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#ifdef NDEBUG
#define EWARN(msg, ...)
#define EINFO(msg, ...)
#define EDEBUG(msg, ...)
#else
#define EWARN(msg, ...)   fprintf(stdout, "[WARN]  %s:%d " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define EINFO(msg, ...)   fprintf(stdout, "[INFO]  %s:%d " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define EDEBUG(msg, ...)  fprintf(stdout, "[DEBUG] %s:%d " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef NDEBUG
#define ERUN_MSG(cmd, msg, ...) (cmd)
#define ERUN(cmd) (cmd)
#define EASSERT_MSG(cond, msg, ...) (cond)
#define EASSERT(cmd) (cmd)
#else
#define ERUN_MSG(cmd, msg, ...) do {            \
        int _ret = (cmd);                       \
        if (_ret != 0)                          \
                EERROR(msg, ##__VA_ARGS__);     \
        } while (0)

#define ERUN(cmd) do {                                                            \
        int _ret = (cmd);                                                         \
        if (_ret != 0)                                                            \
                EERROR("Function '%s' failed with return code %d", #cmd, _ret);   \
        } while (0)
#define EASSERT_MSG(cond, msg, ...) do {                \
        if (!(cond))         {                           \
                EERROR(msg, ##__VA_ARGS__);\
                abort();}\
        } while (0)
#define EASSERT(cond) do {                                                              \
        if (!(cond)) {                                                                  \
                EERROR("Assertion Failed: %s:%d '%s'", __FILE__, __LINE__, #cond);       \
                abort();        \
        }       \
        } while (0)
#endif