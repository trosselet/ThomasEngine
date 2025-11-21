#ifndef RENDER_INCLUDE__H
#define RENDER_INCLUDE__H

#include <Render/Header/UploadBuffer.h>


class Window;
class RenderResources;
class RenderTarget;

class Mesh;
class Material;



struct CameraCB;

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

private:
	RenderResources* m_pRenderResources = nullptr;

    RenderTarget* m_pOffscreenRT = nullptr;

	UploadBuffer<CameraCB>* m_pCbCurrentViewProjInstance;

	friend class GraphicEngine;
};

#endif // !RENDER_INCLUDE__H

