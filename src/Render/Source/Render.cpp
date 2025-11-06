#include "pch.h"
#include "Header/Render.h"
#include "Header/RenderResources.h"
#include "Header/Window.h"

#include "Header/Mesh.h"
#include "Header/Material.h"

Render::Render(const Window* pWindow)
{
	m_pRenderResources = new RenderResources(pWindow->GetHWND(), pWindow->GetWidth(), pWindow->GetHeight());
	m_pRenderResources->Resize(pWindow->GetWidth(), pWindow->GetHeight());

	m_pCbCurrentViewProjInstance = new UploadBuffer<CameraCB>(m_pRenderResources->GetDevice(), 1, 1);
}

Render::~Render()
{
	if (m_pCbCurrentViewProjInstance)
	{
		delete m_pCbCurrentViewProjInstance;
		m_pCbCurrentViewProjInstance = nullptr;
	}

	if (m_pRenderResources)
	{
		delete m_pRenderResources;
		m_pRenderResources = nullptr;
	}
}

void Render::Clear()
{
	if (!m_pRenderResources->ResetCommandList())
		return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pRenderResources->GetCurrentRenderTarget();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_pRenderResources->GetCommandList()->ResourceBarrier(1, &barrier);

	D3D12_VIEWPORT viewport = m_pRenderResources->GetViewport();
	D3D12_RECT rect = m_pRenderResources->GetRect();
	m_pRenderResources->GetCommandList()->RSSetViewports(1, &viewport);
	m_pRenderResources->GetCommandList()->RSSetScissorRects(1, &rect);

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRenderResources->GetCurrentRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pRenderResources->GetCurrentDSV();

	m_pRenderResources->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	m_pRenderResources->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_pRenderResources->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, &rect);

	ID3D12DescriptorHeap* descHeap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
	m_pRenderResources->GetCommandList()->SetDescriptorHeaps(1, &descHeap);
}

void Render::Draw(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
	if (pMesh == nullptr) return;

	pMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&objectWorldMatrix));

	m_pRenderResources->GetCommandList()->SetPipelineState(m_pRenderResources->GetPSO());
	m_pRenderResources->GetCommandList()->SetGraphicsRootSignature(m_pRenderResources->GetRootSignature());

	m_pRenderResources->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW pVbv = pMesh->GetVertexBuffer();
	m_pRenderResources->GetCommandList()->IASetVertexBuffers(0, 1, &pVbv);

	D3D12_INDEX_BUFFER_VIEW pIbv = pMesh->GetIndexBuffer();
	m_pRenderResources->GetCommandList()->IASetIndexBuffer(&pIbv);

	// Pos in the root parameter list of t0
	pMaterial->UpdateTexture(2);

	m_pRenderResources->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
	m_pRenderResources->GetCommandList()->SetGraphicsRootConstantBufferView(1, pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

	m_pRenderResources->GetCommandList()->DrawIndexedInstanced(pMesh->GetIndexCount(), 1, 0, 0, 0);


}

void Render::Display()
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pRenderResources->GetCurrentRenderTarget();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	m_pRenderResources->GetCommandList()->ResourceBarrier(1, &barrier);

	m_pRenderResources->GetCommandList()->Close();
	m_pRenderResources->ExecuteCommandList();

	m_pRenderResources->Present(false);

	m_pRenderResources->WaitForGpu();

	return;
}

RenderResources* Render::GetRenderResources()
{
	return m_pRenderResources;
}
