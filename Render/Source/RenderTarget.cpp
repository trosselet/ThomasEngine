#include <Render/pch.h>
#include <Render/Header/RenderTarget.h>
#include <Render/Header/RenderResources.h>

RenderTarget::RenderTarget(RenderResources* pResources, uint32_t width, uint32_t height, DXGI_FORMAT format, bool createDepth, UINT sampleCount)
	: m_pResources(pResources), m_width(width), m_height(height), m_format(format), m_hasDepth(createDepth), m_pDepth(nullptr), m_pColor(nullptr)
{
	assert(m_pResources != nullptr);

	ID3D12Device* pDevice = m_pResources->GetDevice();
	ID3D12GraphicsCommandList* cmdList = m_pResources->GetCommandList();

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = sampleCount;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;


	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT hr = pDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&m_pColor)
	);

	if (FAILED(hr) || !m_pColor)
	{
		Utils::DebugError("Failed to create RenderTarget color resource");
		assert(hr < 0);
		assert(m_pColor != nullptr);
	}

    m_rtvIndex = m_pResources->AllocateRTV();
    if (m_rtvIndex != UINT_MAX)
    {
        m_rtvCpu = m_pResources->GetRTVHandle(m_rtvIndex);
        m_pResources->GetDevice()->CreateRenderTargetView(m_pColor, nullptr, m_rtvCpu);
    }

	{
		ID3D12DescriptorHeap* heap = m_pResources->GetCbvSrvUavDescriptorHeap();
		UINT increment = m_pResources->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		UINT srvIndex = m_pResources->AllocateSRVHeapIndex();

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += (size_t)srvIndex * increment;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += (size_t)srvIndex * increment;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = m_format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pResources->GetDevice()->CreateShaderResourceView(m_pColor, &srvDesc, cpuHandle);
		m_srvCpu = cpuHandle;
		m_srvGpu = gpuHandle;
	}

	if (m_hasDepth)
	{
		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthDesc.Alignment = 0;
		depthDesc.Width = width;
		depthDesc.Height = height;
		depthDesc.DepthOrArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


		D3D12_CLEAR_VALUE depthClear = {};
		depthClear.Format = DXGI_FORMAT_D32_FLOAT;
		depthClear.DepthStencil.Depth = 1.0f;
		depthClear.DepthStencil.Stencil = 0;


		hr = pDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClear,
			IID_PPV_ARGS(&m_pDepth)
		);


		if (FAILED(hr) || !m_pDepth)
		{
			Utils::DebugError("Failed to create RenderTarget depth resource");
			assert(hr < 0);
			assert(m_pDepth != nullptr);
		}

        {
            m_dsvIndex = m_pResources->AllocateDSV();
            if (m_dsvIndex != UINT_MAX)
            {
                m_dsvCpu = m_pResources->GetDSVHandle(m_dsvIndex);
                m_pResources->GetDevice()->CreateDepthStencilView(m_pDepth, nullptr, m_dsvCpu);
            }
        }
	}

	m_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	m_scissor = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };


	Utils::DebugLog("RT Viewport = ", m_viewport.Width, ", ", m_viewport.Height);

	m_pColor->SetName(L"RenderTarget Color Resource");
	if (m_pDepth)
	{
		m_pDepth->SetName(L"RenderTarget Depth Resource");
	}

}

RenderTarget::~RenderTarget()
{
	if (m_pColor)
	{
		m_pColor->Release();
		m_pColor = nullptr;
	}
	if (m_pDepth)
	{
		m_pDepth->Release();
		m_pDepth = nullptr;
        if (m_dsvIndex != UINT_MAX)
        {
            m_pResources->FreeDSV(m_dsvIndex);
            m_dsvIndex = UINT_MAX;
        }
	}
    if (m_rtvIndex != UINT_MAX)
    {
        m_pResources->FreeRTV(m_rtvIndex);
        m_rtvIndex = UINT_MAX;
    }
}

void RenderTarget::Transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newState)
{
	if (!m_pColor) return;

	if (m_currentState == newState) return;

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColor;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = m_currentState;
	barrier.Transition.StateAfter = newState;

	cmdList->ResourceBarrier(1, &barrier);

	m_currentState = newState;
}
