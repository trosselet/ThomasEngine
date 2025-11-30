#include <Engine/pch.h>

#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/MeshRenderer.h>
#include <Engine/Header/Camera.h>
#include <Engine/Header/Scene.h>
#include <Engine/Header/GameObject.h>

#include <Render/Header/GraphicEngine.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/Material.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Render.h>

#include <Tools/Header/PrimitiveTypes.h>

#include <chrono>
#include <algorithm>
#include <map>
#include <thread>


RenderSystem::RenderSystem(GraphicEngine* pGraphic)
{
	m_pGraphic = pGraphic;


}

void RenderSystem::RenderLayerBatched(const std::vector<MeshRenderer const*>& layer)
{
    std::vector<MeshRenderer const*> sorted = layer;
    std::stable_sort(sorted.begin(), sorted.end(), [](MeshRenderer const* a, MeshRenderer const* b)
    {
        if (a->m_pMaterial != b->m_pMaterial)
            return a->m_pMaterial < b->m_pMaterial;
        return a->m_pMesh < b->m_pMesh;
    });

    auto* res = m_pGraphic->GetRender()->GetRenderResources();
    ID3D12GraphicsCommandList* mainCmd = res->GetCommandList();

    m_pGraphic->BindFrameConstants();

    UINT availableBundles = res->GetBundleCount();
    size_t itemCount = sorted.size();
    if (availableBundles <= 1 || itemCount < 2)
    {
        Material* currentMaterial = nullptr;
        Mesh* currentMesh = nullptr;

        for (MeshRenderer const* mr : sorted)
        {
            if (mr->m_pMaterial != currentMaterial)
            {
                currentMaterial = mr->m_pMaterial;
                if (currentMaterial && currentMaterial->GetTexture())
                {
                    currentMaterial->UpdateTexture(2, mainCmd);
                }
            }

            if (mr->m_pMesh != currentMesh)
            {
                currentMesh = mr->m_pMesh;
                D3D12_VERTEX_BUFFER_VIEW vbv = currentMesh->GetVertexBuffer();
                mainCmd->IASetVertexBuffers(0, 1, &vbv);
                D3D12_INDEX_BUFFER_VIEW ibv = currentMesh->GetIndexBuffer();
                mainCmd->IASetIndexBuffer(&ibv);
            }

            currentMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&mr->m_pOwner->transform.GetMatrixFLOAT()));
            mainCmd->SetGraphicsRootConstantBufferView(1, mr->m_pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());
            mainCmd->DrawIndexedInstanced(mr->m_pMesh->GetIndexCount(), 1, 0, 0, 0);
        }

        return;
    }

    UINT bundleCount = static_cast<UINT>(std::min<size_t>(availableBundles, itemCount));
    std::vector<size_t> chunkStart(bundleCount);
    std::vector<size_t> chunkEnd(bundleCount);
    size_t base = 0;
    for (UINT i = 0; i < bundleCount; ++i)
    {
        size_t start = (itemCount * i) / bundleCount;
        size_t end = (itemCount * (i + 1)) / bundleCount;
        chunkStart[i] = start;
        chunkEnd[i] = end;
    }

    std::vector<std::thread> threads;
    threads.reserve(bundleCount);

    UINT64 frameCBAddr = m_pGraphic->GetFrameCBAddress();

    std::vector<ID3D12GraphicsCommandList*> recordedBundles;
    std::vector<ID3D12CommandAllocator*> recordedAllocs;
    recordedBundles.reserve(bundleCount);
    recordedAllocs.reserve(bundleCount);
    std::mutex recordedMutex;

    for (UINT bi = 0; bi < bundleCount; ++bi)
    {
        threads.emplace_back([&, bi]() {
            ID3D12CommandAllocator* alloc = nullptr;
            ID3D12GraphicsCommandList* bundleCmd = nullptr;
            if (FAILED(res->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&alloc))))
                return;
            if (FAILED(res->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, alloc, nullptr, IID_PPV_ARGS(&bundleCmd))))
            {
                if (alloc) alloc->Release();
                return;
            }

            bundleCmd->SetGraphicsRootSignature(res->GetRootSignature());
            ID3D12DescriptorHeap* descHeap = res->GetCbvSrvUavDescriptorHeap();
            if (descHeap)
                bundleCmd->SetDescriptorHeaps(1, &descHeap);
            bundleCmd->SetPipelineState(res->GetPSO());
            if (frameCBAddr != 0)
                bundleCmd->SetGraphicsRootConstantBufferView(0, frameCBAddr);
            bundleCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            Material* localMaterial = nullptr;
            Mesh* localMesh = nullptr;

            for (size_t idx = chunkStart[bi]; idx < chunkEnd[bi]; ++idx)
            {
                MeshRenderer const* mr = sorted[idx];
                if (mr->m_pMaterial != localMaterial)
                {
                    localMaterial = mr->m_pMaterial;
                    if (localMaterial && localMaterial->GetTexture())
                        localMaterial->UpdateTexture(2, bundleCmd);
                }

                if (mr->m_pMesh != localMesh)
                {
                    localMesh = mr->m_pMesh;
                    D3D12_VERTEX_BUFFER_VIEW vbv = localMesh->GetVertexBuffer();
                    bundleCmd->IASetVertexBuffers(0, 1, &vbv);
                    D3D12_INDEX_BUFFER_VIEW ibv = localMesh->GetIndexBuffer();
                    bundleCmd->IASetIndexBuffer(&ibv);
                }

                localMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&mr->m_pOwner->transform.GetMatrixFLOAT()));
                bundleCmd->SetGraphicsRootConstantBufferView(1, mr->m_pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());
                bundleCmd->DrawIndexedInstanced(mr->m_pMesh->GetIndexCount(), 1, 0, 0, 0);
            }

            bundleCmd->Close();

            {
                std::lock_guard<std::mutex> lock(recordedMutex);
                recordedBundles.push_back(bundleCmd);
                recordedAllocs.push_back(alloc);
            }
        });
    }

    // join threads
    for (auto &t : threads) if (t.joinable()) t.join();

    for (size_t i = 0; i < recordedBundles.size(); ++i)
    {
        ID3D12GraphicsCommandList* bundleCmd = recordedBundles[i];
        ID3D12CommandAllocator* alloc = recordedAllocs[i];
        if (bundleCmd)
            mainCmd->ExecuteBundle(bundleCmd);
        if (bundleCmd) bundleCmd->Release();
        if (alloc) alloc->Release();
    }
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
        std::vector<MeshRenderer const*> active;
        active.reserve(meshRendererLayer.size());
        for (MeshRenderer const* const pMeshRenderer : meshRendererLayer)
        {
            if (pMeshRenderer->m_active)
                active.push_back(pMeshRenderer);
        }

        if (!active.empty())
            RenderLayerBatched(active);
    }
	float renderEndTimeDraw = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTimeDraw).count()) / 1'000'000.0f;
	printf("\033[%d;%dH\033[2K  [ENGINE] Draw time: %f", 8, 0, renderEndTimeDraw);

	auto startTimeDisplay = std::chrono::steady_clock::now();
	m_pGraphic->Display();
	float renderEndTimeDisplay = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTimeDisplay).count()) / 1'000'000.0f;
	printf("\033[%d;%dH\033[2K  [ENGINE] Display time: %f", 9, 0, renderEndTimeDisplay);
}