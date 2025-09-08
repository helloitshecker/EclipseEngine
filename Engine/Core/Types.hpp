#pragma once
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using size = u64;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;
using b32 = i32;
using b64 = i64;
constexpr bool ON = true;
constexpr bool OFF = false;

constexpr u32 GetUpper32(const u64 value) {
        return static_cast<u32>(value >> 32);
}

constexpr u32 GetLower32(const u64 value) {
        return static_cast<u32>(value & 0xFFFFFFFFULL);
}

constexpr u64 SetUpper32(const u32 value) {
        return static_cast<u64>(value) << 32;
}

constexpr u64 SetLower32(const u32 value) {
        return static_cast<u64>(value);
}