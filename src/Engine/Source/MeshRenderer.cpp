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

void MeshRenderer::SetRectangle()
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE, Color::Red);

	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_primitive = true;
}

void MeshRenderer::SetCircle()
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CIRCLE, Color::Red);

	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_primitive = true;
}

void MeshRenderer::SetCube()
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(CUBE, Color::Red);

	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
	m_primitive = true;
}

void MeshRenderer::SetSphere()
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	m_pGeometry = graphics.CreatePrimitiveGeometry(SPHERE, Color::Red);

	m_pMesh = graphics.CreateMesh(m_pGeometry);
	m_pMaterial = graphics.CreateMaterial();
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
