#include "pch.h"
#include "Header/RenderSystem.h"

#include "Header/MeshRenderer.h"

#include "../Render/Header/GraphicEngine.h"

RenderSystem::RenderSystem(GraphicEngine* pGraphic)
{
	m_pGraphic = pGraphic;
}

void RenderSystem::Rendering()
{
	m_pGraphic->BeginDraw();
	for (std::vector<MeshRenderer const*> const& meshRendererLayer : m_meshRenderers)
	{
		for (MeshRenderer const* const pMeshRenderer : meshRendererLayer)
		{
			//if (pMeshRenderer->m_active)
			m_pGraphic->RenderFrame(pMeshRenderer->m_pMesh);

			
		}
	}
	m_pGraphic->Display();
}

void RenderSystem::CreateMesh()
{
	MeshRenderer* mesh = new MeshRenderer();
	mesh->SetRectangle();

	m_meshRenderers[0].push_back(mesh);
}
