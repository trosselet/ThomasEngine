#ifndef RENDER_INCLUDE__H
#define RENDER_INCLUDE__H

class Window;
class RenderResources;

class Mesh;


class Render
{
public:
	Render(const Window* pWindow);
	~Render();

	void Clear();
	void Draw(Mesh* pMesh);
	void Display();

	RenderResources* GetRenderResources();

private:
	RenderResources* m_pRenderResources = nullptr;
};

#endif // !RENDER_INCLUDE__H

