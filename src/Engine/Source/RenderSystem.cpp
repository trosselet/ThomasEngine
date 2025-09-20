#include "pch.h"
#include "Header/RenderSystem.h"

#include "Header/MeshRenderer.h"
#include "Header/Camera.h"

#include "Header/Scene.h"
#include "Header/GameObject.h"

#include "../Render/Header/GraphicEngine.h"

#include "Tools/Header/PrimitiveTypes.h"


RenderSystem::RenderSystem(GraphicEngine* pGraphic)
{
	m_pGraphic = pGraphic;
}

void RenderSystem::Rendering()
{
	m_pGraphic->BeginDraw();

	GameObject* pCamera = GameManager::GetActiveScene().GetMainCamera();
	Camera& cameraComponent = pCamera->GetComponent<Camera>();
	m_pGraphic->UpdateCameraTo(
		Vector3{ pCamera->transform.GetPositionFLOAT() },
		Vector3{ pCamera->transform.GetPositionFLOAT().x + pCamera->transform.Forward().x, pCamera->transform.GetPositionFLOAT().y + pCamera->transform.Forward().y, pCamera->transform.GetPositionFLOAT().z + pCamera->transform.Forward().z },
		Vector3{ pCamera->transform.Up() },
		cameraComponent.viewWidth,
		cameraComponent.viewHeight,
		cameraComponent.fov,
		cameraComponent.nearZ,
		cameraComponent.farZ,
		cameraComponent.projectionMatrix,
		cameraComponent.viewMatrix
	);

	for (std::vector<MeshRenderer const*> const& meshRendererLayer : m_meshRenderers)
	{
		for (MeshRenderer const* const pMeshRenderer : meshRendererLayer)
		{
			if (pMeshRenderer->m_active)
				m_pGraphic->RenderFrame(pMeshRenderer->m_pMesh, pMeshRenderer->m_pMaterial, pMeshRenderer->m_pOwner->transform.GetMatrixFLOAT());
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
