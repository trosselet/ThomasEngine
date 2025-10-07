#ifndef MESH_RENDERER_INCLUDE__H
#define MESH_RENDERER_INCLUDE__H

#include "Engine/Header/RenderSystem.h"
#include "Engine/Header/ComponentBase.h"

struct Geometry;
class Mesh;
class Material;
class Texture;
class Color;

struct MeshRenderer : ComponentBase<Component::MeshRenderer>
{
public:
	~MeshRenderer();

	uint8 renderLayer = 0;

	void SetRectangle(Color c = Color::White);
	void SetRectangle(const char* texturePath, Color c = Color::White);

	void SetCircle(Color c = Color::White);
	void SetCircle(const char* texturePath, Color c = Color::White);

	void SetSphere(Color c = Color::White);
	void SetSphere(const char* texturePath, Color c = Color::White);

	void SetCube(Color c = Color::White);
	void SetCube(const char* texturePath, Color c = Color::White);

	Geometry* GetGeometry();
	Mesh* GetMesh();
	Material* GetMaterial();

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
