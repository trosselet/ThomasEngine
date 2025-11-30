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

    ID3D12GraphicsCommandList* cmdList = m_pRenderResources->GetCommandList();
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (m_pOffscreenRT)
    {
        m_pOffscreenRT->Transition(cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        D3D12_VIEWPORT viewport = m_pOffscreenRT->GetViewport();
        D3D12_RECT rect = m_pOffscreenRT->GetScissor();
        cmdList->RSSetViewports(1, &viewport);
        cmdList->RSSetScissorRects(1, &rect);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pOffscreenRT->GetRTV();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pOffscreenRT->GetDSV();
        cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, (m_pOffscreenRT->HasDepth() ? &dsvHandle : nullptr));
        cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        if (m_pOffscreenRT->HasDepth())
            cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, &rect);

        ID3D12DescriptorHeap* descHeap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &descHeap);

        cmdList->SetPipelineState(m_pRenderResources->GetPostProcessPSO());
        cmdList->SetGraphicsRootSignature(m_pRenderResources->GetPostProcessRootSignature());
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    else
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_pRenderResources->GetCurrentRenderTarget();
        barrier.Transition.Subresource = 0;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        cmdList->ResourceBarrier(1, &barrier);

        D3D12_VIEWPORT viewport = m_pRenderResources->GetViewport();
        D3D12_RECT rect = m_pRenderResources->GetRect();
        cmdList->RSSetViewports(1, &viewport);
        cmdList->RSSetScissorRects(1, &rect);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRenderResources->GetCurrentRTV();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pRenderResources->GetCurrentDSV();
        cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
        cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, &rect);

        ID3D12DescriptorHeap* descHeap = m_pRenderResources->GetCbvSrvUavDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &descHeap);

        cmdList->SetPipelineState(m_pRenderResources->GetPSO());
        cmdList->SetGraphicsRootSignature(m_pRenderResources->GetRootSignature());
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}

void Render::Draw(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
	if (pMesh == nullptr) return;

    // cache command list pointer to avoid repeated lookups
    ID3D12GraphicsCommandList* cmdList = m_pRenderResources->GetCommandList();

    pMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&objectWorldMatrix));

    D3D12_VERTEX_BUFFER_VIEW pVbv = pMesh->GetVertexBuffer();
    cmdList->IASetVertexBuffers(0, 1, &pVbv);

    D3D12_INDEX_BUFFER_VIEW pIbv = pMesh->GetIndexBuffer();
    cmdList->IASetIndexBuffer(&pIbv);


    //////////////////////////////////////////
    // Pos in the root parameter list of t0 //
    //////////////////////////////////////////
    // Update texture descriptor table (material-specific)
    pMaterial->UpdateTexture(2);

    cmdList->SetGraphicsRootConstantBufferView(0, m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
    cmdList->SetGraphicsRootConstantBufferView(1, pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

    cmdList->DrawIndexedInstanced(pMesh->GetIndexCount(), 1, 0, 0, 0);


}

void Render::Display()
{
    ID3D12GraphicsCommandList* cmdList = m_pRenderResources->GetCommandList();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_pRenderResources->GetCurrentRenderTarget();
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    cmdList->ResourceBarrier(1, &barrier);

    cmdList->Close();
    m_pRenderResources->ExecuteCommandList();
    m_pRenderResources->Present(false);
	m_pRenderResources->WaitForGpu();
}

RenderResources* Render::GetRenderResources()
{
	return m_pRenderResources;
}