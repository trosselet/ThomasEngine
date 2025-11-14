#pragma once
#include <Tools/pch.h>

#include <Tools/Header/Vector.hpp>
#include <Tools/Header/Matrix4x4.hpp>

using byte = unsigned char;

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using float32 = float;
using float64 = double;

using Vector2 = Vector<float32, 2>;
using Vector3 = Vector<float32, 3>;
using Vector4 = Vector<float32, 4>;
using Matri4x4 = Matrix4x4;

constexpr float32 PI = 3.14159265358979323846;