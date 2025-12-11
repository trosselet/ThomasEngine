#include <Render/pch.h>
#include <Render/Header/PSOManager.h>
#include <Render/Header/RenderResources.h>

#define SHADER_PATH L"shaders/"

PSOManager::PSOManager(RenderResources* pRenderResources)
{
	m_pRenderResources = pRenderResources;
	CreateRootSignature(m_pRenderResources->GetDevice());
}

PSOManager::~PSOManager()
{
	for (auto& pair : m_psoCache)
	{
		if (pair.second.pPipelineState)
		{
			pair.second.pPipelineState->Release();
			pair.second.pPipelineState = nullptr;
		}
	}
	m_psoCache.clear();

	if (m_pRootSignature)
	{
		m_pRootSignature->Release();
		m_pRootSignature = nullptr;
	}
}

ID3D12PipelineState* PSOManager::GetPSO(const std::wstring& psoPath, const PSOSettings& settings)
{
	std::wstring key = GeneratePSOKey(SHADER_PATH + psoPath, settings.flags);

	auto it = m_psoCache.find(key);
	if (it != m_psoCache.end())
	{
		return it->second.pPipelineState;
	}
	else
	{
		if (CreatePSO(SHADER_PATH + psoPath, settings))
		{
			return m_psoCache[key].pPipelineState;
		}
		else
		{
			return nullptr;
		}
	}
}

ID3D12RootSignature* PSOManager::GetRootSignature()
{
	return m_pRootSignature;
}

bool PSOManager::CreatePSO(const std::wstring& psoPath, const PSOSettings& settings = PSOSettings())
{
	ID3DBlob* vertexShader = CompileShader(psoPath, "vs_5_0");
	ID3DBlob* pixelShader = CompileShader(psoPath, "ps_5_0");

	if (!vertexShader || !pixelShader)
		return false;

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };

	// Rasterizer
	if (Utils::HasFlag(settings.flags, Utils::PSOFlags::Wireframe))
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	else
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}

	if (Utils::HasFlag(settings.flags, Utils::PSOFlags::CullNone))
	{
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else if (Utils::HasFlag(settings.flags, Utils::PSOFlags::CullFront))
	{
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	}
	else
	{
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	}
	psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// Depth stencil
	D3D12_DEPTH_STENCIL_DESC depthDesc = {};

	if (Utils::HasFlag(settings.flags, Utils::PSOFlags::PostProcess))
	{
		depthDesc.DepthEnable = FALSE;
		depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	}
	else
	{
		depthDesc.DepthEnable = Utils::HasFlag(settings.flags, Utils::PSOFlags::DepthEnable);
		depthDesc.DepthWriteMask = Utils::HasFlag(settings.flags, Utils::PSOFlags::DepthWrite) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	}

	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthDesc.StencilEnable = FALSE;
	psoDesc.DepthStencilState = depthDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// Blend
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};

	if (Utils::HasFlag(settings.flags, Utils::PSOFlags::AlphaBlend))		
	{
		blendDesc.BlendEnable = TRUE;
	}
	else
	{
		blendDesc.BlendEnable = FALSE;
	}

	psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	ID3D12PipelineState* pPipelineState = nullptr;
	HRESULT hr = m_pRenderResources->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState));
	if (FAILED(hr))
		return false;

	std::wstring key = GeneratePSOKey(psoPath, settings.flags);
	m_psoCache[key] = { settings, pPipelineState, psoPath };

	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();

	return true;
}

bool PSOManager::CreateRootSignature(ID3D12Device* pDevice)
{
	std::vector<D3D12_ROOT_PARAMETER> rootParams(4);

	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[0].Descriptor.ShaderRegister = 0;
	rootParams[0].Descriptor.RegisterSpace = 0;
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[1].Descriptor.ShaderRegister = 1;
	rootParams[1].Descriptor.RegisterSpace = 0;
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[2].Descriptor.ShaderRegister = 2;
	rootParams[2].Descriptor.RegisterSpace = 0;
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_DESCRIPTOR_RANGE srvRanges[2] = {};
	srvRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRanges[0].NumDescriptors = 64;
	srvRanges[0].BaseShaderRegister = 0;
	srvRanges[0].RegisterSpace = 0;
	srvRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	srvRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRanges[1].NumDescriptors = 16;
	srvRanges[1].BaseShaderRegister = 64;
	srvRanges[1].RegisterSpace = 0;
	srvRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[3].DescriptorTable.NumDescriptorRanges = 2;
	rootParams[3].DescriptorTable.pDescriptorRanges = srvRanges;
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC samplers[16] = {};
	for (int i = 0; i < 16; ++i)
	{
		samplers[i].Filter = D3D12_FILTER_ANISOTROPIC;
		samplers[i].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplers[i].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplers[i].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplers[i].MipLODBias = 0.0f;
		samplers[i].MaxAnisotropy = 16;
		samplers[i].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplers[i].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		samplers[i].MinLOD = 0.0f;
		samplers[i].MaxLOD = D3D12_FLOAT32_MAX;
		samplers[i].ShaderRegister = i;
		samplers[i].RegisterSpace = 0;
		samplers[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
	rootDesc.NumParameters = static_cast<UINT>(rootParams.size());
	rootDesc.pParameters = rootParams.data();
	rootDesc.NumStaticSamplers = _countof(samplers);
	rootDesc.pStaticSamplers = samplers;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (FAILED(hr))
	{
		if (error) OutputDebugStringA((char*)error->GetBufferPointer());
		throw std::runtime_error("Failed to serialize root signature");
	}

	hr = pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	if (FAILED(hr))
		throw std::runtime_error("Failed to create root signature");

	if (signature) signature->Release();
	if (error) error->Release();
}

ID3DBlob* PSOManager::CompileShader(const std::wstring& path, const char* target)
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
