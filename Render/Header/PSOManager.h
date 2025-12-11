#ifndef PSO_MANAGER_INCULDED__H
#define PSO_MANAGER_INCULDED__H


#include <string>
#include <unordered_map>
#include <d3d12.h>

struct PSODesc
{
	ID3D12PipelineState* pPipelineState;
	std::wstring shaderPath;
};

class RenderResources;

class PSOManager
{
public:
	static void Initialize(RenderResources* pRenderResources)
	{
		if (!m_pInstance)
		{
			m_pInstance = new PSOManager(pRenderResources);
		}
	}

	static PSOManager* GetInstance()
	{
		return m_pInstance;
	}

public:
	PSOManager(RenderResources* pRenderResources);
	~PSOManager();

	ID3D12PipelineState* GetPSO(const std::wstring& psoPath);

	ID3D12RootSignature* GetRootSignature();

private:
	bool CreatePSO(const std::wstring& psoPath);
	bool CreateRootSignature(ID3D12Device* pDevice);
	ID3DBlob* CompileShader(const std::wstring& path, const char* target);

private:
	RenderResources* m_pRenderResources;
	std::unordered_map<std::wstring, PSODesc> m_psoCache;
	ID3D12RootSignature* m_pRootSignature;

private:
	inline static PSOManager* m_pInstance = nullptr;
};




#endif // !PSO_MANAGER_INCULDED__H
