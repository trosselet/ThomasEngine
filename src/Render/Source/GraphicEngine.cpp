#include "pch.h"
#include "Header/GraphicEngine.h"

#include "Header/Window.h"
#include "Header/Render.h"

#include "Header/PrimitiveGeometry.h"
#include "Header/Geometry.h"

#include "Header/RenderResources.h"
#include "Header/Mesh.h"

GraphicEngine::GraphicEngine(const Window* pWindow)
{
	m_pRender = new Render(pWindow);
	PrimitiveGeometry::InitializeGeometry();
}

GraphicEngine::~GraphicEngine()
{
	delete m_pRender;
}

void GraphicEngine::BeginDraw()
{
	m_pRender->Clear();
}

void GraphicEngine::RenderFrame(Mesh* pMesh)
{
	m_pRender->Draw(pMesh);
}

void GraphicEngine::Display()
{
	m_pRender->Display();
}

Geometry* GraphicEngine::CreatePrimitiveGeometry(PrimitiveGeometryType primitiveType, Color color)
{
	Geometry const* const pGeo = PrimitiveGeometry::GetPrimitiveGeometry(primitiveType);
	Geometry* pResult = new Geometry();
	*pResult = *pGeo;

	for (int i = 0; i < pResult->positions.size(); i++)
	{
		pResult->colors.push_back(DirectX::XMFLOAT4(color.r, color.g, color.b, color.a));
	}

	return pResult;
}

Mesh* GraphicEngine::CreateMesh(Geometry* pGeometry)
{
	m_pRender->GetRenderResources()->ResetCommandList();
	Mesh* mesh = new Mesh(pGeometry, m_pRender);
	m_pRender->GetRenderResources()->GetCommandList()->Close();
	m_pRender->GetRenderResources()->ExecuteCommandList();
	m_pRender->GetRenderResources()->FlushQueue();
	mesh->ReleaseUploadBuffers();
	return mesh;
}
