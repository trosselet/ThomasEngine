#ifndef DXGI_RESOURCES__H
#define DXGI_RESOURCES__H

class IDXGIFactory2;
class IDXGIAdapter;
class ID3D12Device;
class IDXGISwapChain3;
class ID3D12Resource;
class ID3D12CommandAllocator;
class ID3D12CommandQueue;
class ID3D12RootSignature;
class ID3D12DescriptorHeap;
class ID3D12PipelineState;
class ID3D12GraphicsCommandList;
class ID3D12Fence;

class RenderResources
{
public:
	RenderResources(HWND hwnd, uint32 width, uint32 height);
	~RenderResources();

	IDXGIFactory2* GetDXGIFactory();
	IDXGIAdapter* GetDXGIAdapters();

private:
	void CreateDXGIFactory();
	void CreateDXGIAdapters();

	void CreateDevice(IDXGIAdapter* pAdapter);
	void CreateCommandQueue(ID3D12Device* pDevice);
	void CreateSwapChain(IDXGIFactory2* pFactory, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, HWND hwnd, uint32 width, uint32 height);
	void CreateDescriptorHeap(ID3D12Device* pDevice);
	void CreateRenderTargets(ID3D12Device* pDevice);
	void CreateCommandAllocator(ID3D12Device* pDevice);
	void CreateRootSignature(ID3D12Device* pDevice);
	ID3DBlob* CompileShader(const std::wstring& path, const char* target);
	void CreatePipelineState(ID3D12Device* pDevice, const std::wstring& shaderPath);
	void CreateCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pcmdAllocator, ID3D12PipelineState* pPso);

private:
	IDXGIFactory2* m_pFactory = nullptr;
	IDXGIAdapter* m_pAdapter = nullptr;
	IDXGISwapChain3* m_pSwapChain = nullptr;

private:
	static const UINT FrameCount = 2;

private:
	ID3D12Device* m_pDevice = nullptr;
	ID3D12Resource* m_pRenderTargets[FrameCount];
	ID3D12CommandAllocator* m_pCommandAllocator;
	ID3D12CommandQueue* m_pCommandQueue;
	ID3D12RootSignature* m_pRootSignature;
	ID3D12DescriptorHeap* m_pRtvHeap;
	ID3D12PipelineState* m_pPipelineState;
	ID3D12GraphicsCommandList* m_pCommandList;
	UINT m_rtvDescriptorSize;

private:
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ID3D12Fence* m_pFence;
	UINT64 m_fenceValue;

};

#endif // !DXGI_RESOURCES__H
