#ifndef DXGI_RESOURCES__H
#define DXGI_RESOURCES__H

#include <vector>

struct IDXGIFactory2;
struct IDXGIAdapter;
struct ID3D12Device;
struct IDXGISwapChain3;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12RootSignature;
struct ID3D12DescriptorHeap;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

struct tagRECT;
struct D3D12_VIEWPORT;

class RenderTarget;

class RenderResources
{
public:
    RenderResources(HWND hwnd, uint32 width, uint32 height);
    ~RenderResources();

    IDXGIFactory2* GetDXGIFactory();
    IDXGIAdapter* GetDXGIAdapters();

    ID3D12Device* GetDevice();
    ID3D12GraphicsCommandList* GetCommandList();
    D3D12_RECT GetRect();
    D3D12_VIEWPORT GetViewport();
    RenderTarget* GetCurrentRenderTarget();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDSV();
    ID3D12DescriptorHeap* GetCbvSrvUavDescriptorHeap();
    ID3D12PipelineState* GetPSO();
    ID3D12RootSignature* GetRootSignature();

    ID3D12Resource* CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ID3D12Resource** uploadBuffer, D3D12_RESOURCE_STATES finalState);

    void WaitForGpu();
    void Resize(UINT width, UINT height);
    void ExecuteCommandList();
    bool ResetCommandList();
    void FlushQueue();
    void Present(bool vsync);

    UINT AllocateSRVHeapIndex() { return m_srvHeapIndex++; }

    D3D12_CPU_DESCRIPTOR_HANDLE CreateRTV(ID3D12Resource* resource);
    D3D12_CPU_DESCRIPTOR_HANDLE CreateDSV(ID3D12Resource* resource);
    void CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE& outCpu, D3D12_GPU_DESCRIPTOR_HANDLE& outGpu);


private:
    void CreateDXGIFactory();
    void CreateDXGIAdapters();
    void CreateDevice(IDXGIAdapter* pAdapter);
    void CreateCommandQueue(ID3D12Device* pDevice);
    void CreateSwapChain(IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, HWND hwnd, uint32 width, uint32 height);
    void CreateDescriptorHeap(ID3D12Device* pDevice);
    void CreateRenderTargets(UINT width, UINT height);
    void CreateCommandAllocator(ID3D12Device* pDevice);
    void CreateRootSignature(ID3D12Device* pDevice);
    ID3DBlob* CompileShader(const std::wstring& path, const char* target);
    void CreatePipelineState(ID3D12Device* pDevice, const std::wstring& shaderPath);
    void CreateCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pcmdAllocator, ID3D12PipelineState* pPso);
    void CreateCbvSrvUavDescriptorHeap();
    void CreateFence(ID3D12Device* pDevice);
    void UpdateViewport(uint32 width, uint32 height);

private:
    IDXGIFactory2* m_pFactory = nullptr;
    IDXGIAdapter* m_pAdapter = nullptr;
    IDXGISwapChain3* m_pSwapChain = nullptr;

    static const UINT FrameCount = 2;

private:
    ID3D12Device* m_pDevice = nullptr;
    std::vector<RenderTarget*> m_renderTargets;
    ID3D12CommandAllocator* m_pCommandAllocator = nullptr;
    ID3D12CommandQueue* m_pCommandQueue = nullptr;
    ID3D12RootSignature* m_pRootSignature = nullptr;
    ID3D12DescriptorHeap* m_pRtvHeap = nullptr;
    ID3D12DescriptorHeap* m_pDsvDescriptorHeap = nullptr;
    ID3D12DescriptorHeap* m_pCbvSrvUavDescriptorHeap = nullptr;
    ID3D12PipelineState* m_pPipelineState = nullptr;
    ID3D12GraphicsCommandList* m_pCommandList = nullptr;

    UINT m_rtvDescriptorSize = 0;
    UINT m_dsvDescriptorSize = 0;
    UINT m_cbvSrvUavDescriptorSize = 0;
    uint32 m_srvHeapIndex = 0;
    
    UINT m_nextCbvSrvUavIndex = 0;
    UINT m_nextRtvIndex = 0;
    UINT m_nextDsvIndex = 0;

    UINT m_frameIndex = 0;
    HANDLE m_fenceEvent = nullptr;
    ID3D12Fence* m_pFence = nullptr;
    UINT64 m_fenceValue = 0;

    D3D12_RECT m_scissorRect;
    D3D12_VIEWPORT m_screenViewport;
};

#endif // DXGI_RESOURCES__H