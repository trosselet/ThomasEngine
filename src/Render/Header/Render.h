#ifndef RENDER_INCLUDE__H
#define RENDER_INCLUDE__H

class Window;
class RenderResources;

class Render
{
public:
	Render(const Window* pWindow);
	~Render();

	void Clear();
	void Draw();
	void Display();

private:
	RenderResources* m_pRenderResources = nullptr;
};

#endif // !RENDER_INCLUDE__H

