#ifndef RENDERTARGET_INCLUDED__H
#define RENDERTARGET_INCLUDED__H

class RenderResources;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

class RenderTarget
{
public:
    RenderTarget(RenderResources* pResources, uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool createDepth = true, UINT sampleCount = 1);
    ~RenderTarget();


    ID3D12Resource* GetResource() const { return m_pColor; }


    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_rtvCpu; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return m_dsvCpu; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpu() const { return m_srvCpu; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvGpu; }


    D3D12_VIEWPORT GetViewport() const { return m_viewport; }
    D3D12_RECT GetScissor() const { return m_scissor; }


    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }


    bool HasDepth() const { return m_hasDepth; }

    void Transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newState);

private:
    RenderResources* m_pResources = nullptr;


    ID3D12Resource* m_pColor;
    ID3D12Resource* m_pDepth;


    D3D12_CPU_DESCRIPTOR_HANDLE m_rtvCpu = {};
    D3D12_CPU_DESCRIPTOR_HANDLE m_dsvCpu = {};
    D3D12_CPU_DESCRIPTOR_HANDLE m_srvCpu = {};
    D3D12_GPU_DESCRIPTOR_HANDLE m_srvGpu = {};


    uint32_t m_width = 0;
    uint32_t m_height = 0;
    DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bool m_hasDepth = false;


    D3D12_VIEWPORT m_viewport = {};
    D3D12_RECT m_scissor = {};

    D3D12_RESOURCE_STATES m_currentState;

};


#endif // !RENDERTARGET_INCLUDED__H
