#ifndef RENDER_INCLUDE__H
#define RENDER_INCLUDE__H

#include <Render/Header/UploadBuffer.h>


class Window;
class RenderResources;
class RenderTarget;

class Mesh;
class Material;



struct CameraCB;



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

    void SetOffscreenRenderTarget(RenderTarget* rt) { m_pOffscreenRT = rt; }

	void Clear();
	void Draw(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix);
	void Display();

	RenderResources* GetRenderResources();

	void SetNeedsResizeWindow() { m_needsResizeWindow = true; }
	void SetResizeWindow(WindowResizeInfo resize) { m_resizeInfo = resize; }

	void SetNeedsResizeRT() { m_needsResizeRT = true; }
	void SetResizeRT(WindowResizeInfo resize) { m_resizeRTInfo = resize; }

	void SetWireframe(bool wireframe);

private:
	void ResizeWindow();

private:
	RenderResources* m_pRenderResources = nullptr;

    RenderTarget* m_pOffscreenRT = nullptr;

	UploadBuffer<CameraCB>* m_pCbCurrentViewProjInstance;

	bool m_needsResizeWindow = false;
	WindowResizeInfo m_resizeInfo = {};
	bool m_needsResizeRT = false;
	WindowResizeInfo m_resizeRTInfo = {};

	bool m_isWireframe = false;

	friend class GraphicEngine;
};

#endif // !RENDER_INCLUDE__H

