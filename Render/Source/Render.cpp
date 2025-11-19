#include <Render/pch.h>

#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/RenderTarget.h>
#include <Render/Header/Window.h>

#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>


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

void Render::Clear(RenderTarget* target)
{
	if (!target)
		return;

	if (!m_pRenderResources->ResetCommandList())
		return;

	target->Transition(m_pRenderResources->GetCommandList(), D3D12_RESOURCE_STATE_RENDER_TARGET);


	D3D12_VIEWPORT viewport = target->GetViewport();
	D3D12_RECT scissor = target->GetScissor();
	m_pRenderResources->GetCommandList()->RSSetViewports(1, &viewport);
	m_pRenderResources->GetCommandList()->RSSetScissorRects(1, &scissor);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = target->GetRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = target->HasDepth() ? target->GetDSV() : D3D12_CPU_DESCRIPTOR_HANDLE{};
	m_pRenderResources->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, target->HasDepth() ? &dsv : nullptr);

	const float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
	m_pRenderResources->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	if (target->HasDepth())
		m_pRenderResources->GetCommandList()->ClearDepthStencilView(dsv,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 1, &scissor);

	ID3D12DescriptorHeap* descHeap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
	m_pRenderResources->GetCommandList()->SetDescriptorHeaps(1, &descHeap);
}

void Render::Draw(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
	if (!pMesh || !pMaterial) return;

	RenderResources* res = m_pRenderResources;

	pMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&objectWorldMatrix));

	ID3D12GraphicsCommandList* cmd = res->GetCommandList();

	cmd->SetPipelineState(res->GetPSO());
	cmd->SetGraphicsRootSignature(res->GetRootSignature());

	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW vbv = pMesh->GetVertexBuffer();
	cmd->IASetVertexBuffers(0, 1, &vbv);

	D3D12_INDEX_BUFFER_VIEW ibv = pMesh->GetIndexBuffer();
	cmd->IASetIndexBuffer(&ibv);

	// Root CBV and textures
	pMaterial->UpdateTexture(2);
	cmd->SetGraphicsRootConstantBufferView(0, m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

	cmd->DrawIndexedInstanced(pMesh->GetIndexCount(), 1, 0, 0, 0);


}

void Render::Display()
{
	RenderResources* res = m_pRenderResources;
	RenderTarget* backBufferRT = res->GetCurrentRenderTarget();
	ID3D12GraphicsCommandList* cmdList = res->GetCommandList();

	backBufferRT->Transition(cmdList, D3D12_RESOURCE_STATE_PRESENT);

	cmdList->Close();
	res->ExecuteCommandList();

	res->Present(false);
	res->WaitForGpu();
}

void Render::BeginRenderTarget(RenderTarget* pRT)
{
	if (!pRT) return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = pRT->GetResource();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_pRenderResources->GetCommandList()->ResourceBarrier(1, &barrier);


	D3D12_CPU_DESCRIPTOR_HANDLE rtv = pRT->GetRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = pRT->GetDSV();


	m_pRenderResources->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, pRT->HasDepth() ? &dsv : nullptr);

	//////////////////////////////
	// Set viewport and scissor //
	//////////////////////////////

	D3D12_VIEWPORT vp = pRT->GetViewport();
	D3D12_RECT sc = pRT->GetScissor();
	m_pRenderResources->GetCommandList()->RSSetViewports(1, &vp);
	m_pRenderResources->GetCommandList()->RSSetScissorRects(1, &sc);

	///////////////////////////
	// Bind descriptor heaps //
	///////////////////////////

	ID3D12DescriptorHeap* heaps[] = { m_pRenderResources->GetCbvSrvUavDescriptorHeap() };
	m_pRenderResources->GetCommandList()->SetDescriptorHeaps(1, heaps);

	/////////////////////
	// Clear RTV + DSV //
	/////////////////////

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pRenderResources->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	if (pRT->HasDepth())
	{
		m_pRenderResources->GetCommandList()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
}

void Render::EndRenderTarget(RenderTarget* pRT)
{
	if (!pRT) return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = pRT->GetResource();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_pRenderResources->GetCommandList()->ResourceBarrier(1, &barrier);
}

RenderResources* Render::GetRenderResources()
{
	return m_pRenderResources;
}
