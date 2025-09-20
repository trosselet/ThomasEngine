#ifndef MESH_RENDERER_INCLUDE__H
#define MESH_RENDERER_INCLUDE__H

#include "Engine/Header/RenderSystem.h"
#include "Engine/Header/ComponentBase.h"

struct Geometry;
class Mesh;
class Material;

struct MeshRenderer : ComponentBase<Component::MeshRenderer>
{
public:
	~MeshRenderer();

	uint8 renderLayer = 0;

	void SetRectangle();

private:
	void Free();

private:

	bool m_primitive = false;
	Geometry* m_pGeometry = nullptr;
	Mesh* m_pMesh = nullptr;
	Material* m_pMaterial = nullptr;


private:
	friend class RenderSystem;
};

#endif // !MESH_RENDERER_INCLUDE__H
