#ifndef RENDERTARGET_INCLUDED__H
#define RENDERTARGET_INCLUDED__H

class RenderResources;

struct RenderTargetDesc
{
    uint32_t width;
    uint32_t height;
    DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN;
    UINT sampleCount = 1;
};

class RenderTarget
{
public:
    RenderTarget(RenderResources* resources, const RenderTargetDesc& desc);
    ~RenderTarget();

    ID3D12Resource* GetColor() const                            { return m_color; }
    ID3D12Resource* GetDepth() const                            { return m_depth; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const                  { return m_rtvCpu; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const                  { return m_dsvCpu; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const                  { return m_srvGpu; }

    const D3D12_VIEWPORT& GetViewport() const                   { return m_viewport; }
    const D3D12_RECT& GetScissor() const                        { return m_scissor; }

    bool HasDepth() const                                       { return m_depth != nullptr; }

    D3D12_RESOURCE_STATES GetCurrentRTRenderState() const       { return m_currentRTState; }
    void SetCurrentRTRenderState(D3D12_RESOURCE_STATES state)   { m_currentRTState = state; }

private:
    void CreateColor();
    void CreateDepth();
    void CreateDescriptors();

private:
    RenderResources* m_resources = nullptr;
    RenderTargetDesc m_desc = {};

    ID3D12Resource* m_color = nullptr;
    ID3D12Resource* m_depth = nullptr;

    D3D12_RESOURCE_STATES m_currentRTState = D3D12_RESOURCE_STATE_COMMON;

    UINT m_rtvIndex = UINT_MAX;
    UINT m_dsvIndex = UINT_MAX;
    UINT m_srvIndex = UINT_MAX;

    D3D12_CPU_DESCRIPTOR_HANDLE m_rtvCpu = {};
    D3D12_CPU_DESCRIPTOR_HANDLE m_dsvCpu = {};
    D3D12_GPU_DESCRIPTOR_HANDLE m_srvGpu = {};

    D3D12_VIEWPORT m_viewport = {};
    D3D12_RECT m_scissor = {};
};

#endif
