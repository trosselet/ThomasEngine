#include "pch.h"
#include "Header/MeshRenderer.h"
#include "Header/RenderSystem.h"
#include "Header/GameManager.h"

#include "Render/Header/Geometry.h"
#include "Render/Header/PrimitiveGeometry.h"
#include "Render/Header/Mesh.h"
#include "Render/Header/Material.h"
#include "Render/Header/GraphicEngine.h"
#include "Render/Header/Window.h"


#include "../Tools/Header/Color.h"

MeshRenderer::~MeshRenderer()
{
	Free();
}

void MeshRenderer::SetRectangle(Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE, c);


	m_pTexture = graphics.CreateTexture("DefaultTex.dds");
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::SetRectangle(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE, c);


	m_pTexture = graphics.CreateTexture(texturePath);
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
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
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CUBE, c);

	m_pTexture = graphics.CreateTexture("DefaultTex.dds");
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
	m_primitive = true;
}

void MeshRenderer::SetCube(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CUBE, c);

	m_pTexture = graphics.CreateTexture(texturePath);
	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture);
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

	if (m_pGeometry)
	{
		delete m_pGeometry;
		m_pGeometry = nullptr;
	}

	if (m_pMaterial)
	{
		delete m_pMaterial;
		m_pMaterial = nullptr;
	}

	if (m_pMesh)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}

	m_primitive = false;
}
