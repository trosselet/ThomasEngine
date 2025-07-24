#pragma once

#include "CommonHeaders.h"

namespace thomas::math
{
	constexpr float pi = 3.1415926535897932384626433832795f;
	constexpr float epsilon = 1e-5f;

#if defined(_WIN64)

	using vector2f = DirectX::XMFLOAT2;
	using vector2Af = DirectX::XMFLOAT2A;
	using vector3f = DirectX::XMFLOAT3;
	using vector3Af = DirectX::XMFLOAT3A;
	using vector4f = DirectX::XMFLOAT4;
	using vector4Af = DirectX::XMFLOAT4A;

	using uint32v2 = DirectX::XMUINT2;
	using uint32v3 = DirectX::XMUINT3;
	using uint32v4 = DirectX::XMUINT4;
	
	using int32v2 = DirectX::XMINT2;
	using int32v3 = DirectX::XMINT3;
	using int32v4 = DirectX::XMINT4;

	using matrix3x3 = DirectX::XMFLOAT3X3;
	using matrix4X4 = DirectX::XMFLOAT4X4;
	using matrix4X4a = DirectX::XMFLOAT4X4A;

#endif
}