#pragma once

#include <stdint.h>

// uint

using uint8  = uint8_t ;  // 0 -> 255
using uint16 = uint16_t;  // 0 -> 511
using uint32 = uint32_t;  // 0 -> 1023
using uint64 = uint64_t;  // 0 -> 2047

// signed int

using sint8  = int8_t ;
using sint16 = int16_t;
using sint32 = int32_t;
using sint64 = int64_t;

constexpr uint64 uint64_invalid_id{ 0xffff'ffff'ffff'ffffui64 };
constexpr uint32 uint32_invalid_id{ 0xffff'ffffui32 };
constexpr uint16 uint16_invalid_id{ 0xffffui16 };
constexpr uint8  uint8_invalid_id { 0xffui8  };

using float32 = float;