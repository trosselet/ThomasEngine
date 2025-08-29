#ifndef RENDER_SYSTEM_INCLUDE__H
#define RENDER_SYSTEM_INCLUDE__H

class GraphicEngine;

class RenderSystem
{
public:
	RenderSystem(GraphicEngine* pGraphic);
	void Rendering();

private:

	GraphicEngine* m_pGraphic;
};

#endif // !RENDER_SYSTEM_INCLUDE__H
