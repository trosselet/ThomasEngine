#ifndef PRIMITIVE_GEOMETRY_INCLUDE__H
#define PRIMITIVE_GEOMETRY_INCLUDE__H

#include <vector>

#include <Render/Header/GraphicEngine.h>

struct Geometry;

enum PrimitiveGeometryType : uint8
{
	SQUARE,
	CIRCLE,
	CUBE,
	SPHERE,

	PRIMITIVE_COUNT
};

class PrimitiveGeometry
{
public:
	static PrimitiveGeometry* Get();
	static Geometry* GetPrimitiveGeometry(PrimitiveGeometryType primitiveType);

private:
	static void InitializeGeometry();

private:
	std::vector<Geometry> m_geometries;

private:
	friend class GraphicEngine;
};


#endif // !PRIMITIVE_GEOMETRY_INCLUDE__H