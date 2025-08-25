#ifndef GRAPHIC_ENGINE_INCLUDE__H
#define GRAPHIC_ENGINE_INCLUDE__H

class Window;
class Render;

class GraphicEngine
{
public:
	GraphicEngine(const Window* pWindow);
	~GraphicEngine();

	void BeginDraw();
	void RenderFrame();
	void Display();

private:
	Render* m_pRender = nullptr;
};

#endif // !GRAPHIC_ENGINE_INCLUDE__H
