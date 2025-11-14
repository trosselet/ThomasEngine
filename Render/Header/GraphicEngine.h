#ifndef GRAPHIC_ENGINE_INCLUDE__H
#define GRAPHIC_ENGINE_INCLUDE__H

#include <Tools/Header/Color.h>
#include <DirectXMath.h>

#include <Render/Header/ResourcesCache.h>
#include <vector>

class Window;
class Render;

struct Geometry;
enum PrimitiveGeometryType : uint8;

class Mesh;
class Material;
class Texture;

class GraphicEngine
{
public:
	GraphicEngine(const Window* pWindow);
	~GraphicEngine();

	void BeginDraw();
	void RenderFrame(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix);
	void Display();

	Geometry* CreatePrimitiveGeometry(PrimitiveGeometryType primitiveType, Color color);
	Geometry* CreateGeometryFromFile(const char* meshPath, const char* extension, Color color = Color::White);
	Mesh* CreateMesh(Geometry* pGeometry);
	Material* CreateMaterial();
	Texture* CreateTexture(char const* filePath);

	void UpdateCameraAt(Vector3 const& position, Vector3 const& target, Vector3 const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix);
	void UpdateCameraTo(Vector3 const& position, Vector3 const& target, Vector3 const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix);

	Mesh* CreateMeshDeferred(Geometry* pGeometry);
	void ProcessPendingUploads();


	Render* GetRender();

public:

	std::vector<Mesh*> m_pendingMeshUploads;

	ResourcesCache<Texture> m_textureCache;
	ResourcesCache<Geometry> m_geometryCache;
	ResourcesCache<Mesh> m_meshCache;

private:
	Render* m_pRender = nullptr;
};

#endif // !GRAPHIC_ENGINE_INCLUDE__H
