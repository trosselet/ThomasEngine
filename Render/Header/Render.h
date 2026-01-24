#ifndef RENDER_INCLUDE__H
#define RENDER_INCLUDE__H

#include <Render/Header/UploadBuffer.h>

class Window;
class RenderResources;
class RenderTarget;
class Mesh;
class Material;

struct WindowResizeInfo
{
    uint32 width;
    uint32 height;
};

class Render
{
public:
    Render(const Window* pWindow);
    ~Render();

    void SetOffscreenRenderTarget(RenderTarget* rt);

    void BeginFrame();
    void Draw(Mesh* mesh, Material* material, DirectX::XMFLOAT4X4 const& world);
    void EndFrame();

    RenderResources* GetRenderResources();

    void RequestResizeWindow(WindowResizeInfo info);
    void RequestResizeRT(WindowResizeInfo info);

    void SetWireframe(bool wireframe);

    UploadBuffer<CameraCB>* GetCameraCB() const { return m_cameraCB; };
	RenderTarget* GetOffscreenRenderTarget() const { return m_offscreenRT; };

private:
    void ResizeIfNeeded();

    void Transition(ID3D12GraphicsCommandList* cmd, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, RenderTarget* pRenderTarget = nullptr);

private:
    RenderResources* m_resources = nullptr;
    RenderTarget* m_offscreenRT = nullptr;

    UploadBuffer<CameraCB>* m_cameraCB = nullptr;

    bool m_needsResizeWindow = false;
    bool m_needsResizeRT = false;

    WindowResizeInfo m_windowResize = {};
    WindowResizeInfo m_rtResize = {};

	

    bool m_wireframe = false;
};

#endif
