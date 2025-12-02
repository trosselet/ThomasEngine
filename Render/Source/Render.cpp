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

void Render::Clear()
{
    if (!m_pRenderResources->ResetCommandList())
        return;

    ID3D12GraphicsCommandList* cmd = m_pRenderResources->GetCommandList();

    const float clearColor[4] = { 0,0,0,1 };
     
    m_pOffscreenRT->Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
     

	D3D12_VIEWPORT viewport = m_pOffscreenRT->GetViewport();
	D3D12_RECT scissor = m_pOffscreenRT->GetScissor();


    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_pOffscreenRT->GetRTV();
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_pOffscreenRT->GetDSV();

    cmd->OMSetRenderTargets(1, &rtv, FALSE,
        m_pOffscreenRT->HasDepth() ? &dsv : nullptr);

    cmd->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

    if (m_pOffscreenRT->HasDepth())
    {
        cmd->ClearDepthStencilView(dsv,
            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            1.0f, 0, 0, nullptr);
    }
     
    ID3D12DescriptorHeap* heap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
    cmd->SetDescriptorHeaps(1, &heap);

    cmd->SetPipelineState(m_pRenderResources->GetPSO());
    cmd->SetGraphicsRootSignature(m_pRenderResources->GetRootSignature());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Render::Draw(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
    if (!pMesh) return;

    ID3D12GraphicsCommandList* cmd = m_pRenderResources->GetCommandList();

    // Upload matrices
    pMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&objectWorldMatrix));

    // VB / IB
    auto vb = pMesh->GetVertexBuffer();
    auto ib = pMesh->GetIndexBuffer();
    cmd->IASetVertexBuffers(0, 1, &vb);
    cmd->IASetIndexBuffer(&ib);

    // SRV t2
    pMaterial->UpdateTexture(2);

    // CBV root params
    cmd->SetGraphicsRootConstantBufferView(0, m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
    cmd->SetGraphicsRootConstantBufferView(1, pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

    cmd->DrawIndexedInstanced(pMesh->GetIndexCount(), 1, 0, 0, 0);


}

void Render::Display()
{
    ID3D12GraphicsCommandList* cmd = m_pRenderResources->GetCommandList();

    m_pOffscreenRT->Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    auto backBuffer = m_pRenderResources->GetCurrentRenderTarget();

    D3D12_RESOURCE_BARRIER toRT = {};
    toRT.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toRT.Transition.pResource = backBuffer;
    toRT.Transition.Subresource = 0;
    toRT.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    toRT.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    cmd->ResourceBarrier(1, &toRT);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_pRenderResources->GetCurrentRTV();
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_pRenderResources->GetCurrentDSV();

    cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

    D3D12_VIEWPORT viewport = m_pRenderResources->GetViewport();
    D3D12_RECT scissor = m_pRenderResources->GetRect();

    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);

    ID3D12DescriptorHeap* heap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
    cmd->SetDescriptorHeaps(1, &heap);

    cmd->SetPipelineState(m_pRenderResources->GetPostProcessPSO());
    cmd->SetGraphicsRootSignature(m_pRenderResources->GetPostProcessRootSignature());
    cmd->SetGraphicsRootDescriptorTable(0, m_pOffscreenRT->GetSRV());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    cmd->DrawInstanced(3, 1, 0, 0);

    D3D12_RESOURCE_BARRIER toPresent = {};
    toPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toPresent.Transition.pResource = backBuffer;
    toPresent.Transition.Subresource = 0;
    toPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    toPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    cmd->ResourceBarrier(1, &toPresent);

    cmd->Close();
    m_pRenderResources->ExecuteCommandList();
    m_pRenderResources->Present(false);
    m_pRenderResources->WaitForGpu();
}

RenderResources* Render::GetRenderResources()
{
	return m_pRenderResources;
}