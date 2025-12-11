#ifndef PSO_MANAGER_INCULDED__H
#define PSO_MANAGER_INCULDED__H


#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <Tools/Header/Utils.h>

struct PSOSettings
{
	Utils::PSOFlags flags = Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite;
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;

	D3D12_FILL_MODE GetFillMode() const
	{
		return HasFlag(flags, Utils::PSOFlags::Wireframe) ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	}

	D3D12_CULL_MODE GetCullMode() const
	{
		return HasFlag(flags, Utils::PSOFlags::CullNone) ? D3D12_CULL_MODE_NONE : D3D12_CULL_MODE_BACK;
	}

	bool IsDepthEnabled() const { return HasFlag(flags, Utils::PSOFlags::DepthEnable); }
	bool IsDepthWrite() const { return HasFlag(flags, Utils::PSOFlags::DepthWrite); }
	bool IsAlphaBlend() const { return HasFlag(flags, Utils::PSOFlags::AlphaBlend); }
};

struct PSODesc
{
	PSOSettings settings;
	ID3D12PipelineState* pPipelineState;
	std::wstring shaderPath;
};

class RenderResources;

class PSOManager
{
public:
	static inline void Initialize(RenderResources* pRenderResources)
	{
		if (!m_pInstance)
		{
			m_pInstance = new PSOManager(pRenderResources);
		}
	}

	static inline PSOManager* GetInstance()
	{
		return m_pInstance;
	}

public:
	PSOManager(RenderResources* pRenderResources);
	~PSOManager();

	ID3D12PipelineState* GetPSO(const std::wstring& psoPath, const PSOSettings& settings = PSOSettings());

	ID3D12RootSignature* GetRootSignature();

private:
	bool CreatePSO(const std::wstring& psoPath, const PSOSettings& settings);
	bool CreateRootSignature(ID3D12Device* pDevice);
	ID3DBlob* CompileShader(const std::wstring& path, const char* target);
	inline std::wstring GeneratePSOKey(const std::wstring& shaderPath, Utils::PSOFlags flags)
	{
		return shaderPath + L"#" + std::to_wstring(static_cast<uint32_t>(flags));
	}

private:
	RenderResources* m_pRenderResources;
	std::unordered_map<std::wstring, PSODesc> m_psoCache;
	ID3D12RootSignature* m_pRootSignature;

private:
	inline static PSOManager* m_pInstance = nullptr;
};




#endif // !PSO_MANAGER_INCULDED__H
