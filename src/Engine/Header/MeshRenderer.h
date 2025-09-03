#ifndef MESH_RENDERER_INCLUDE__H
#define MESH_RENDERER_INCLUDE__H

#include "Header/RenderSystem.h"

struct Geometry;
class Mesh;

class MeshRenderer
{
public:
	~MeshRenderer();

	void SetRectangle();

private:
	void Free();

private:

	bool m_primitive = false;
	Geometry* m_pGeometry = nullptr;
	Mesh* m_pMesh = nullptr;

private:
	friend class RenderSystem;
};

#endif // !MESH_RENDERER_INCLUDE__H
