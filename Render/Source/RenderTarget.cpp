#include <Render/pch.h>
#include <Render/Header/RenderTarget.h>
#include <Render/Header/RenderResources.h>

RenderTarget::RenderTarget(RenderResources* resources, const RenderTargetDesc& desc) : m_resources(resources), m_desc(desc)
{
    assert(m_resources != nullptr);

    CreateColor();

    if (m_desc.depthFormat != DXGI_FORMAT_UNKNOWN)
        CreateDepth();

    CreateDescriptors();

    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = (float)m_desc.width;
    m_viewport.Height = (float)m_desc.height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissor.left = 0;
    m_scissor.top = 0;
    m_scissor.right = (LONG)m_desc.width;
    m_scissor.bottom = (LONG)m_desc.height;
}

RenderTarget::~RenderTarget()
{
    if (m_color)
    {
        m_color->Release();
        Utils::DebugInfo("Release m_color !");
        m_color = nullptr;
    }

    if (m_depth)
    {
        m_depth->Release();
        m_depth = nullptr;
    }

    if (m_rtvIndex != UINT_MAX)
        m_resources->FreeRTV(m_rtvIndex);

    if (m_dsvIndex != UINT_MAX)
        m_resources->FreeDSV(m_dsvIndex);
}

void RenderTarget::CreateColor()
{
    ID3D12Device* device = m_resources->GetDevice();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = m_desc.width;
    desc.Height = m_desc.height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = m_desc.colorFormat;
    desc.SampleDesc.Count = m_desc.sampleCount;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clear = {};
    clear.Format = m_desc.colorFormat;
    clear.Color[3] = 1.0f;

    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        &clear,
        IID_PPV_ARGS(&m_color)
    );

	m_color->SetName(L"RenderTarget Color Resource");

    if (FAILED(hr) || !m_color)
    {
        Utils::DebugError("Failed to create RenderTarget color resource");
        assert(hr < 0);
        assert(m_color != nullptr);
    }
    else
    {
        Utils::DebugInfo("Create m_color !");
    }
}

void RenderTarget::CreateDepth()
{
    ID3D12Device* device = m_resources->GetDevice();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = m_desc.width;
    desc.Height = m_desc.height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = m_desc.depthFormat;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear = {};
    clear.Format = m_desc.depthFormat;
    clear.DepthStencil.Depth = 1.0f;

    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        &clear,
        IID_PPV_ARGS(&m_depth)
    );

	m_depth->SetName(L"RenderTarget Depth Resource");

    if (FAILED(hr) || !m_color)
    {
        Utils::DebugError("Failed to create RenderTarget depth resource");
        assert(hr < 0);
        assert(m_color != nullptr);
    }
    else
    {
        Utils::DebugInfo("Create m_depth !");
    }
}

void RenderTarget::CreateDescriptors()
{
    ID3D12Device* device = m_resources->GetDevice();

    // RTV
    m_rtvIndex = m_resources->AllocateRTV();
    m_rtvCpu = m_resources->GetRTVHandle(m_rtvIndex);
    device->CreateRenderTargetView(m_color, nullptr, m_rtvCpu);

    // SRV
    m_srvIndex = m_resources->AllocateSRVHeapIndex();

    UINT inc = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    ID3D12DescriptorHeap* heap = m_resources->GetCbvSrvUavDescriptorHeap();

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = heap->GetCPUDescriptorHandleForHeapStart();
    cpu.ptr += (size_t)m_srvIndex * inc;

    m_srvGpu = heap->GetGPUDescriptorHandleForHeapStart();
    m_srvGpu.ptr += (size_t)m_srvIndex * inc;

    D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
    srv.Format = m_desc.colorFormat;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(m_color, &srv, cpu);

    // DSV
    if (m_depth)
    {
        m_dsvIndex = m_resources->AllocateDSV();
        m_dsvCpu = m_resources->GetDSVHandle(m_dsvIndex);
        device->CreateDepthStencilView(m_depth, nullptr, m_dsvCpu);
    }
}
