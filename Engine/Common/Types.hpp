#pragma once

#ifdef ECL_ALIGNALL
using u64 = unsigned long;
using u32 = unsigned long;
using u16 = unsigned long;
using u8  = unsigned char;

using i64 = long;
using i32 = long;
using i16 = long;
using i8  = char;
#else
using u64 = unsigned long;
using u32 = unsigned int;
using u16 = unsigned short;
using u8  = unsigned char;

using i64 = long;
using i32 = int;
using i16 = short;
using i8  = char;
#endif