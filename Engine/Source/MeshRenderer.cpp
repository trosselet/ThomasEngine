#include <Engine/pch.h>
#include <Engine/Header/MeshRenderer.h>
#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/GameManager.h>

#include <Render/Header/Geometry.h>
#include <Render/Header/PrimitiveGeometry.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>
#include <Render/Header/GraphicEngine.h>
#include <Render/Header/Window.h>


#include <Tools/Header/Color.h>
#include <filesystem>

MeshRenderer::~MeshRenderer()
{
	Free();
}

void MeshRenderer::SetRectangle(Color c)
{
	SetRectangle("DefaultTex.dds", c);
}

void MeshRenderer::SetRectangle(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();


	std::string geomKey = "primitive:rect";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(SQUARE, c);
		});
	m_ownsGeometry = false;


	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texturePath);
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

void MeshRenderer::SetCircle(Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CIRCLE, c);

	m_pTexture = graphics.CreateTexture("DefaultTex.dds");
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::SetCircle(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CIRCLE, c);

	m_pTexture = graphics.CreateTexture(texturePath);
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::SetCube(Color c)
{
	SetCube("DefaultTex.dds", c);
}

void MeshRenderer::SetCube(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();


	std::string geomKey = "primitive:cube";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(CUBE, c);
		});
	m_ownsGeometry = false;


	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texturePath);
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

void MeshRenderer::SetMeshFile(const char* objPath)
{
	SetMeshFileInternal(objPath, "DefaultTex.dds");
}

void MeshRenderer::SetMeshFile(const char* objPath, const char* texturePath)
{
	SetMeshFileInternal(objPath, texturePath);
}

void MeshRenderer::SetMeshFileInternal(const char* objPath, const char* texturePath)
{
	Free();
	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	std::string extension = std::filesystem::path(objPath).extension().string();

	std::string geomKey = extension + ":" + objPath;
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreateGeometryFromFile(objPath, extension.c_str());
		});
	m_ownsGeometry = false;


	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texturePath);
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

Geometry* MeshRenderer::GetGeometry()
{
	return m_pGeometry;
}

Mesh* MeshRenderer::GetMesh()
{
	return m_pMesh;
}

Material* MeshRenderer::GetMaterial()
{
	return m_pMaterial;
}

void MeshRenderer::SetSphere(Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SPHERE, c);

	m_pTexture = graphics.CreateTexture("DefaultTex.dds");
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::SetSphere(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SPHERE, c);

	m_pTexture = graphics.CreateTexture(texturePath);
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::Free()
{
	if (m_primitive == false) return;

	if (m_pGeometry && m_ownsGeometry)
	{
		delete m_pGeometry;
	}

	if (m_pMaterial)
	{
		delete m_pMaterial;
	}

	if (m_pMesh && m_ownsMesh)
	{
		delete m_pMesh;
	}

	m_pGeometry = nullptr;
	m_pMaterial = nullptr;
	m_pMesh = nullptr;

	m_primitive = false;
}
