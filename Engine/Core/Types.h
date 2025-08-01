#pragma once

// Null Pointer
#define nullptr ((void*)0)

#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define uintptr uintptr_t
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t
#define f32 float
#define f64 double
#define b32 i32
#define b64 i64
#define thread_local _Thread_local

#define ESUCCESS 0
#define EFAILURE 1

typedef enum {
         GL,VK,METAL
} EApi;

typedef struct {
        char* content;          ///<Contains raw file content
        u64 contentSize;        ///<Contains File Size (real file size + 1) (for null terminator)
} EString;