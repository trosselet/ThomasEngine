#include <Render/pch.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/RenderTarget.h>

#if _EXE

#define SHADER_PATH L"shaders/shader.hlsl"

#else

#define SHADER_PATH L"shaders/shader.hlsl"

#endif

RenderResources::RenderResources(HWND hwnd, uint32 width, uint32 height)
{
	CreateDXGIFactory();
	CreateDXGIAdapters();
	CreateDevice(m_pAdapter);
	CreateCommandQueue(m_pDevice);
	CreateDescriptorHeap(m_pDevice);
	CreateSwapChain(m_pFactory, m_pCommandQueue, hwnd, width, height);
	CreateFence(m_pDevice);
	CreateCbvSrvUavDescriptorHeap();

	m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateRenderTargets(width, height);
	CreateCommandAllocator(m_pDevice);

	CreatePipelineState(m_pDevice, SHADER_PATH);
	CreateCommandList(m_pDevice, m_pCommandAllocator, m_pPipelineState);
}

RenderResources::~RenderResources()
{
	WaitForGpu();

	for (auto rt : m_renderTargets)
		delete rt;

	m_renderTargets.clear();

	if (m_pCommandList) m_pCommandList->Release();
	if (m_pPipelineState) m_pPipelineState->Release();
	if (m_pRootSignature) m_pRootSignature->Release();
	if (m_pCommandAllocator) m_pCommandAllocator->Release();
	if (m_pCbvSrvUavDescriptorHeap) m_pCbvSrvUavDescriptorHeap->Release();
	if (m_pRtvHeap) m_pRtvHeap->Release();
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pCommandQueue) m_pCommandQueue->Release();
	if (m_pFence) m_pFence->Release();
	if (m_fenceEvent) CloseHandle(m_fenceEvent);
	if (m_pDevice) m_pDevice->Release();
	if (m_pAdapter) m_pAdapter->Release();
	if (m_pFactory) m_pFactory->Release();

	IDXGIDebug1* dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		dxgiDebug->Release();
	}
}

void RenderResources::WaitForGpu()
{
	if (m_pCommandQueue && m_pFence)
	{
		UINT64 fenceValue = m_fenceValue + 1;
		if (SUCCEEDED(m_pCommandQueue->Signal(m_pFence, fenceValue)))
		{
			m_fenceValue = fenceValue;
			if (m_pFence->GetCompletedValue() < fenceValue)
			{
				if (m_fenceEvent)
				{
					m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
					WaitForSingleObject(m_fenceEvent, INFINITE);
				}
			}
		}
	}
}

void RenderResources::Resize(UINT width, UINT height)
{
	WaitForGpu();

	m_nextRtvIndex = 0;
	m_nextDsvIndex = 0;
	m_nextCbvSrvUavIndex = 0;

	CreateRenderTargets(width, height);
	UpdateViewport(width, height);
}

ID3D12Resource* RenderResources::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	ID3D12Resource** uploadBuffer,
	D3D12_RESOURCE_STATES finalState)
{
	ID3D12Resource* defaultBuffer = nullptr;

	/*====================*/
	/*  DEFAULT heap(GPU) */
	/*====================*/
	
	D3D12_HEAP_PROPERTIES defaultHeapProps = {};
	defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = byteSize;
	bufferDesc.Height = 1;
	bufferDesc.Alignment = 0;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device->CreateCommittedResource(
		&defaultHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&defaultBuffer)
	);
	if (FAILED(hr)) return nullptr;
	/*===================*/
	/*  UPLOAD heap(CPU) */
	/*===================*/
	 
	D3D12_HEAP_PROPERTIES uploadHeapProps = {};
	uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	hr = device->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer)
	);
	if (FAILED(hr)) {
		defaultBuffer->Release();
		return nullptr;
	}

	/*==========================================*/
	/* Transition DEFAULT : COMMON -> COPY_DEST */
	/*==========================================*/

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = defaultBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	cmdList->ResourceBarrier(1, &barrier);

	/*==========================*/
	/* Copy CPU -> UploadBuffer */
	/*==========================*/

	void* mappedData = nullptr;
	D3D12_RANGE readRange = {};
	hr = (*uploadBuffer)->Map(0, &readRange, &mappedData);
	if (initData != nullptr)
	{
		memcpy(mappedData, initData, byteSize);
	}
	(*uploadBuffer)->Unmap(0, nullptr);

	/*==============================*/
	/* Copy GPU : Upload -> Default */
	/*==============================*/

	cmdList->CopyBufferRegion(defaultBuffer, 0, *uploadBuffer, 0, byteSize);

	/*============================================*/
	/* Transition DEFAULT : COPY_DEST->finalState */
	/*============================================*/

	barrier.Transition.pResource = defaultBuffer;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = finalState;
	cmdList->ResourceBarrier(1, &barrier);

	if (FAILED(hr))
	{
		(*uploadBuffer)->Release();
		defaultBuffer->Release();
		return nullptr;
	}


	return defaultBuffer;
}


void RenderResources::ExecuteCommandList()
{
	ID3D12CommandList* const cmdLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, cmdLists);
	m_fenceValue++;
	m_pCommandQueue->Signal(m_pFence, m_fenceValue);
}

bool RenderResources::ResetCommandList()
{
	m_pCommandAllocator->Reset();
	GetCommandList()->Reset(m_pCommandAllocator, 0);

	return true;
}

void RenderResources::FlushQueue()
{
	if (!m_pCommandQueue || !m_pFence || !m_fenceEvent)
		return;

	m_fenceValue++;
	UINT64 fenceToWait = m_fenceValue;

	if (FAILED(m_pCommandQueue->Signal(m_pFence, fenceToWait)))
	{
		Utils::DebugError("Failed to signal fence in FlushQueue.\n");
		return;
	}

	if (m_pFence->GetCompletedValue() < fenceToWait)
	{
		if (FAILED(m_pFence->SetEventOnCompletion(fenceToWait, m_fenceEvent)))
		{
			Utils::DebugError("Failed to set event on completion in FlushQueue.\n");
			return;
		}

		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void RenderResources::Present(bool vsync)
{
	m_pSwapChain->Present(vsync, 0);

	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderResources::CreateRTV(ID3D12Resource* resource)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (size_t)(m_rtvDescriptorSize * m_nextRtvIndex);

	m_pDevice->CreateRenderTargetView(resource, nullptr, handle);

	m_nextRtvIndex++;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderResources::CreateDSV(ID3D12Resource* resource)
{
	if (!m_pDsvDescriptorHeap)
	{
		Utils::DebugError("DSV Heap missing when creating DSV");
		return D3D12_CPU_DESCRIPTOR_HANDLE{};
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (size_t)m_dsvDescriptorSize * m_nextDsvIndex;

	m_pDevice->CreateDepthStencilView(resource, nullptr, handle);

	m_nextDsvIndex++;
	return handle;
}

void RenderResources::CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE& outCpu, D3D12_GPU_DESCRIPTOR_HANDLE& outGpu)
{
	if (!m_pCbvSrvUavDescriptorHeap)
	{
		Utils::DebugError("CBV_SRV_UAV heap missing when creating SRV");
		return;
	}


	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (size_t)m_cbvSrvUavDescriptorSize * m_nextCbvSrvUavIndex;


	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_pCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (size_t)m_cbvSrvUavDescriptorSize * m_nextCbvSrvUavIndex;


	m_pDevice->CreateShaderResourceView(resource, srvDesc, cpuHandle);


	outCpu = cpuHandle;
	outGpu = gpuHandle;


	m_nextCbvSrvUavIndex++;
}

IDXGIFactory2* RenderResources::GetDXGIFactory()
{
	return m_pFactory;
}

IDXGIAdapter* RenderResources::GetDXGIAdapters()
{
	return m_pAdapter;
}

ID3D12Device* RenderResources::GetDevice()
{
	return m_pDevice;
}

ID3D12GraphicsCommandList* RenderResources::GetCommandList()
{
	return m_pCommandList;
}

D3D12_RECT RenderResources::GetRect()
{
	return m_scissorRect;
}

D3D12_VIEWPORT RenderResources::GetViewport()
{
	return m_screenViewport;
}

RenderTarget* RenderResources::GetCurrentRenderTarget()
{
	return m_renderTargets[m_frameIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderResources::GetCurrentRTV()
{
	return m_renderTargets[m_frameIndex]->GetRTV();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderResources::GetCurrentDSV()
{
	return m_renderTargets[m_frameIndex]->HasDepth() ? m_renderTargets[m_frameIndex]->GetDSV() : D3D12_CPU_DESCRIPTOR_HANDLE{};
}

ID3D12DescriptorHeap* RenderResources::GetCbvSrvUavDescriptorHeap()
{
	return m_pCbvSrvUavDescriptorHeap;
}

ID3D12PipelineState* RenderResources::GetPSO()
{
	return m_pPipelineState;
}

ID3D12RootSignature* RenderResources::GetRootSignature()
{
	return m_pRootSignature;
}

void RenderResources::CreateDXGIFactory()
{
#if defined(_DEBUG)
	{
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	if (SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_pFactory))))
	{
		Utils::DebugLog("DXGI Factory has been created !");
		return;
	}
}

void RenderResources::CreateDXGIAdapters()
{
	IDXGIFactory6* pFactory6;

	if (SUCCEEDED(m_pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6))))
	{
		if (FAILED(pFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_pAdapter))))
		{
			Utils::DebugError("Error finding the adapter");
		}

		pFactory6->Release();
	}
	else
	{
		Utils::DebugError("Adapter not compatible with factory 6");
	}
}

void RenderResources::CreateDevice(IDXGIAdapter* pAdapter)
{
	if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_pDevice))))
	{
		Utils::DebugLog("Device has successfuly been created !");
		return;
	}
	Utils::DebugError("Error while creating the device !");
}

void RenderResources::CreateCommandQueue(ID3D12Device* pDevice)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	if (SUCCEEDED(pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue))))
	{
		Utils::DebugLog("Command queue has been created !");
		return;
	}

	Utils::DebugError("Error while creating the command queue !");
}

void RenderResources::CreateSwapChain(IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, HWND hwnd, uint32 width, uint32 height)
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = FrameCount;
	desc.Scaling = DXGI_SCALING_NONE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain1* pSwapChain1 = nullptr;

	if (SUCCEEDED(pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &desc, nullptr, nullptr, &pSwapChain1)))
	{
		if (SUCCEEDED(pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pSwapChain)))
		{
			Utils::DebugLog("SwapChain has successfuly been created !");
			m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
			pSwapChain1->Release();
			return;
		}
		Utils::DebugError("Error while casting the swap chain from swap chain 1 to 3 !");
		pSwapChain1->Release();
		return;
	}
	Utils::DebugError("Error while creating the swap chain !");
}

void RenderResources::CreateDescriptorHeap(ID3D12Device* pDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NumDescriptors = FrameCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (SUCCEEDED(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRtvHeap))))
	{
		Utils::DebugLog("RTV descriptor heap has been created !");
		return;
	}
	Utils::DebugError("Error while creating the RTV descriptor heap !");
}

void RenderResources::CreateRenderTargets(UINT width, UINT height)
{
	for (auto rt : m_renderTargets)
		delete rt;

	m_renderTargets.clear();

	for (UINT i = 0; i < FrameCount; ++i)
	{
		RenderTarget* rt = new RenderTarget(this, width, height);
		m_renderTargets.push_back(rt);
	}

	m_frameIndex = 0;
}

void RenderResources::CreateCommandAllocator(ID3D12Device* pDevice)
{
	if (SUCCEEDED(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator))))
	{
		Utils::DebugLog("Command Allocator has been created !");
		return;
	}
	Utils::DebugError("Error while creating the Command Allocator !");
}

void RenderResources::CreateRootSignature(ID3D12Device* pDevice)
{
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	//////////////////////////////
	// cbPass to b0 (View/Proj) //
	//////////////////////////////

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0; // b0
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	///////////////////////////////
	// cbPerObject to b1 (World) //
	///////////////////////////////

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1; // b1
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	///////////////////////////////
	//		 Texture to t0       //
	///////////////////////////////
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	D3D12_DESCRIPTOR_RANGE srvRange = {};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;
	srvRange.BaseShaderRegister = 0; // t0
	srvRange.RegisterSpace = 0;
	srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &srvRange;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;  // s0
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
	rootDesc.NumParameters = 3;
	rootDesc.pParameters = rootParameters;
	rootDesc.NumStaticSamplers = 1;
	D3D12_STATIC_SAMPLER_DESC samplers[] = { samplerDesc };
	rootDesc.pStaticSamplers = samplers;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (FAILED(hr))
	{
		if (error)
			Utils::DebugError("Root Signature Serialize Error: ", (const char*)error->GetBufferPointer());
	}
	else
	{
		hr = m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		if (FAILED(hr))
			Utils::DebugError("Error creating Root Signature!");
		else
			Utils::DebugLog("Root Signature created with CBVs !");
	}

	if (signature) signature->Release();
	if (error) error->Release();

}


ID3DBlob* RenderResources::CompileShader(const std::wstring& path, const char* target)
{
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

	const char* entryPoint = (!target || strcmp(target, "vs_5_0") != 0) ? "psmain" : "vsmain";

	ID3DBlob* compiledShader;
	ID3DBlob* errorBlob;

	HRESULT hr = D3DCompileFromFile(path.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, flags, 0, &compiledShader, &errorBlob);

	if (FAILED(hr))
	{
		Utils::DebugError("Shader loading error was: ", hr, ", for shader: ", path);
		if (errorBlob)
		{
			Utils::DebugError("Shader compilation error: ", (const char*)errorBlob->GetBufferPointer(), ", for shader :", path);
		}
		return nullptr;
	}

	return compiledShader;
}

void RenderResources::CreatePipelineState(ID3D12Device* pDevice, const std::wstring& shaderPath)
{
	if (!pDevice)
	{
		Utils::DebugError(" PSO device was not initialized !");
		return;
	}

	ID3DBlob* vsBlob = CompileShader(shaderPath, "vs_5_0");
	ID3DBlob* psBlob = CompileShader(shaderPath, "ps_5_0");

	CreateRootSignature(pDevice);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputLayout , _countof(inputLayout)};
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };


	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	if (FAILED(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState))))
	{
		Utils::DebugError("Error failed to create pso !");
		return;
	}
}

void RenderResources::CreateCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pcmdAllocator, ID3D12PipelineState* pPso)
{
	if (SUCCEEDED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pcmdAllocator, pPso, IID_PPV_ARGS(&m_pCommandList))))
	{
		m_pCommandList->Close();
		Utils::DebugLog("CommandList successfuly created !");
		return;
	}
}

void RenderResources::CreateCbvSrvUavDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1000;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;

	if (FAILED(m_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pCbvSrvUavDescriptorHeap))))
	{
		Utils::DebugError("Failed to create CBV/SRV/UAV descriptor heap.");
	}
}

void RenderResources::CreateFence(ID3D12Device* pDevice)
{
	if (m_pDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) != S_OK)
	{
		Utils::DebugError("Error creating the Device Resources fence, at line: ", __LINE__, ", At file: ", __FILE__);
	}
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void RenderResources::UpdateViewport(uint32 width, uint32 height)
{
	m_screenViewport = { 0.0f, 0.0f, static_cast<float32>(width), static_cast<float32>(height), 0.0f, 1.0f };
	m_scissorRect = { 0, 0, static_cast<uint16>(width), static_cast<uint16>(height) };
}
