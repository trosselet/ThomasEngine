#ifndef GEOMETRY_INCLUDE__H
#define GEOMETRY_INCLUDE__H

#include <DirectXMath.h>
#include <vector>
#include <Tools/Header/PrimitiveTypes.h>

struct Geometry
{
	std::vector<uint32> indicies;

	uint32 indexNumber;

	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT4> colors{};
	std::vector<DirectX::XMFLOAT2> UVs{};
	std::vector<DirectX::XMFLOAT3> normals{};

};

#endif // !GEOMETRY_INCLUDE__H
