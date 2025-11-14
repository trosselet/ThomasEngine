#ifndef MESH_RENDERER_INCLUDE__H
#define MESH_RENDERER_INCLUDE__H

#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/ComponentBase.h>

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

	void SetRectangle(Color c);
	void SetRectangle(const char* texturePath, Color c);

	void SetCircle(Color c);
	void SetCircle(const char* texturePath, Color c);

	void SetSphere(Color c);
	void SetSphere(const char* texturePath, Color c);

	void SetCube(Color c);
	void SetCube(const char* texturePath, Color c);

	void SetMeshFile(const char* objPath);
	void SetMeshFile(const char* objPath, const char* texturePath);

	void SetMeshFileInternal(const char* objPath, const char* texturePath);

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
	bool m_ownsGeometry = true;
	bool m_ownsMesh = true;
	bool m_ownsMaterial = true;

private:
	friend class RenderSystem;
};

#endif // !MESH_RENDERER_INCLUDE__H
