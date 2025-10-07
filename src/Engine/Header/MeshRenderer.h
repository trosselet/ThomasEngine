#ifndef MESH_RENDERER_INCLUDE__H
#define MESH_RENDERER_INCLUDE__H

#include "Engine/Header/RenderSystem.h"
#include "Engine/Header/ComponentBase.h"

struct Geometry;
class Mesh;
class Material;
class Texture;

struct MeshRenderer : ComponentBase<Component::MeshRenderer>
{
public:
	~MeshRenderer();

	uint8 renderLayer = 0;

	void SetRectangle();
	void SetRectangle(const char* texturePath);

	void SetCircle();
	void SetCircle(const char* texturePath);

	void SetSphere();
	void SetSphere(const char* texturePath);

	void SetCube();
	void SetCube(const char* texturePath);

private:
	void Free();

private:

	bool m_primitive = false;
	Geometry* m_pGeometry = nullptr;
	Mesh* m_pMesh = nullptr;
	Material* m_pMaterial = nullptr;
	Texture* m_pTexture = nullptr;

private:
	friend class RenderSystem;
};

#endif // !MESH_RENDERER_INCLUDE__H
