#include <Engine/pch.h>

#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/MeshRenderer.h>
#include <Engine/Header/Camera.h>
#include <Engine/Header/Scene.h>
#include <Engine/Header/GameObject.h>

#include <Render/Header/GraphicEngine.h>

#include <Tools/Header/PrimitiveTypes.h>

#include <chrono>


RenderSystem::RenderSystem(GraphicEngine* pGraphic)
{
	m_pGraphic = pGraphic;


}

void RenderSystem::Rendering()
{
	auto startTimeClear = std::chrono::steady_clock::now();
	m_pGraphic->BeginDraw();
	float renderEndTimeClear = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTimeClear).count()) / 1'000'000.0f;
	printf("\033[%d;%dH\033[2K  [ENGINE] Clear time: %f", 7, 0, renderEndTimeClear);

	GameObject* pCamera = GameManager::GetActiveScene().GetMainCamera();
	Camera& cameraComponent = *pCamera->GetComponent<Camera>();
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

	auto startTimeDraw = std::chrono::steady_clock::now();
	for (std::vector<MeshRenderer const*> const& meshRendererLayer : m_meshRenderers)
	{
		for (MeshRenderer const* const pMeshRenderer : meshRendererLayer)
		{
			if (pMeshRenderer->m_active)
				m_pGraphic->RenderFrame(pMeshRenderer->m_pMesh, pMeshRenderer->m_pMaterial, pMeshRenderer->m_pOwner->transform.GetMatrixFLOAT());
		}
	}
	float renderEndTimeDraw = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTimeDraw).count()) / 1'000'000.0f;
	printf("\033[%d;%dH\033[2K  [ENGINE] Draw time: %f", 8, 0, renderEndTimeDraw);

	auto startTimeDisplay = std::chrono::steady_clock::now();
	m_pGraphic->Display();
	float renderEndTimeDisplay = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTimeDisplay).count()) / 1'000'000.0f;
	printf("\033[%d;%dH\033[2K  [ENGINE] Display time: %f", 9, 0, renderEndTimeDisplay);
}