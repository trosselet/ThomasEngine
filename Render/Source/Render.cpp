#include <Render/pch.h>

#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/RenderTarget.h>
#include <Render/Header/Window.h>
#include <Render/Header/PSOManager.h>

#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>


Render::Render(const Window* window)
{
    m_resources = NEW RenderResources(window->GetHWND(),window->GetWidth(),window->GetHeight());

    m_resources->Resize(window->GetWidth(), window->GetHeight());

    m_cameraCB = NEW UploadBuffer<CameraCB>(m_resources->GetDevice(), 1, 1);
}


Render::~Render()
{
    if (m_cameraCB)
    {
        delete m_cameraCB;
        m_cameraCB = nullptr;
    }

    if (m_resources)
        m_resources->WaitForGpu();

    if (m_offscreenRT)
    {
        delete m_offscreenRT;
        m_offscreenRT = nullptr;
    }

    if (m_resources)
    {
        delete m_resources;
        m_resources = nullptr;
    }
}


void Render::BeginFrame()
{
    ResizeIfNeeded();

    if (!m_resources->ResetCommandList())
        return;

    ID3D12GraphicsCommandList* cmd = m_resources->GetCommandList();

	if (!m_offscreenRT)
        return;

	if (m_offscreenRT->GetCurrentRTRenderState() != D3D12_RESOURCE_STATE_COMMON)
    {
        Transition(cmd, m_offscreenRT->GetColor(), m_offscreenRT->GetCurrentRTRenderState(), D3D12_RESOURCE_STATE_COMMON, m_offscreenRT);
    }
    // Transition RT offscreen
    Transition(cmd, m_offscreenRT->GetColor(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET, m_offscreenRT);

    // Setup viewport / scissor
    cmd->RSSetViewports(1, &m_offscreenRT->GetViewport());
    cmd->RSSetScissorRects(1, &m_offscreenRT->GetScissor());

    // Bind RT
    D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_offscreenRT->GetRTV();
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_offscreenRT->HasDepth() ? m_offscreenRT->GetDSV() : D3D12_CPU_DESCRIPTOR_HANDLE{};

    cmd->OMSetRenderTargets(1, &rtv, FALSE,m_offscreenRT->HasDepth() ? &dsv : nullptr);

    // Clear
    const float clearColor[4] = { 0, 0, 0, 1 };
    cmd->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

    if (m_offscreenRT->HasDepth())
        cmd->ClearDepthStencilView(dsv,D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Root & heaps
    ID3D12DescriptorHeap* heap = m_resources->GetCbvSrvUavDescriptorHeap();

    cmd->SetDescriptorHeaps(1, &heap);
    cmd->SetGraphicsRootSignature(PSOManager::GetInstance()->GetRootSignature());

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void Render::Draw( Mesh* mesh, Material* material, DirectX::XMFLOAT4X4 const& world)
{
    if (!mesh || !material)
        return;

    ID3D12GraphicsCommandList* cmd = m_resources->GetCommandList();

    material->sSetWireframe = m_wireframe;
    cmd->SetPipelineState(material->GetPSO());

    material->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&world));
    material->UpdateMaterialConstantBuffer();

    auto vb = mesh->GetVertexBuffer();
    auto ib = mesh->GetIndexBuffer();

    cmd->IASetVertexBuffers(0, 1, &vb);
    cmd->IASetIndexBuffer(&ib);

    material->UpdateTexture(3, 2);

    cmd->SetGraphicsRootConstantBufferView( 0, m_cameraCB->GetResource()->GetGPUVirtualAddress());

    cmd->SetGraphicsRootConstantBufferView( 1, material->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

    cmd->DrawIndexedInstanced( mesh->GetIndexCount(), 1, 0, 0, 0);
}


void Render::EndFrame()
{
    ID3D12GraphicsCommandList* cmd = m_resources->GetCommandList();

    // Offscreen -> SRV
    Transition(cmd, m_offscreenRT->GetColor(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, m_offscreenRT);

    // Backbuffer -> RT
    ID3D12Resource* backBuffer = m_resources->GetCurrentRenderTarget();

    Transition(cmd, backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Bind backbuffer
    D3D12_CPU_DESCRIPTOR_HANDLE bbRTV = m_resources->GetCurrentRTV();

    cmd->OMSetRenderTargets(1, &bbRTV, FALSE, nullptr);

	auto viewport = m_resources->GetViewport();
	auto rect = m_resources->GetRect();

    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &rect);

    // Post-process PSO
    PSOSettings pso = {};
    pso.flags = Utils::PSOFlags::PostProcess;

    cmd->SetPipelineState(PSOManager::GetInstance()->GetPSO( L"postProcess.hlsl", pso));

    cmd->SetGraphicsRootDescriptorTable(3, m_offscreenRT->GetSRV());

    cmd->DrawInstanced(3, 1, 0, 0);

    // Backbuffer -> Present
    Transition(cmd, backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    cmd->Close();
    m_resources->ExecuteCommandList();
    m_resources->Present(false);
    m_resources->WaitForGpu();
    
}


RenderResources* Render::GetRenderResources()
{
	return m_resources;
}

void Render::SetWireframe(bool wireframe)
{
	m_wireframe = wireframe;
}

void Render::ResizeIfNeeded()
{
    if (m_needsResizeWindow)
    {
        m_needsResizeWindow = false;
        m_resources->Resize(m_windowResize.width, m_windowResize.height);
    }

    if (m_needsResizeRT)
    {
        m_needsResizeRT = false;

        m_resources->WaitForGpu();

        delete m_offscreenRT;
        m_offscreenRT = nullptr;

        RenderTargetDesc desc = {};
        desc.width = m_rtResize.width;
        desc.height = m_rtResize.height;
        desc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.depthFormat = DXGI_FORMAT_D32_FLOAT;

        m_offscreenRT = NEW RenderTarget(m_resources, desc);
    }
}


void Render::Transition(ID3D12GraphicsCommandList* cmd, ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, RenderTarget* pRenderTarget)
{
    if (before == after)
        return;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = res;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;

    if (pRenderTarget != nullptr)
    {
		pRenderTarget->SetCurrentRTRenderState(after);
	}

    cmd->ResourceBarrier(1, &barrier);
}

void Render::SetOffscreenRenderTarget(RenderTarget* rt)
{
    m_offscreenRT = rt;
}

void Render::RequestResizeWindow(WindowResizeInfo info)
{
    m_windowResize = info;
    m_needsResizeWindow = true;
}

void Render::RequestResizeRT(WindowResizeInfo info)
{
    m_rtResize = info;
    m_needsResizeRT = true;
}
