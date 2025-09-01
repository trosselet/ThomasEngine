#ifndef RENDER_SYSTEM_INCLUDE__H
#define RENDER_SYSTEM_INCLUDE__H

#include <array>
#include <vector>

class GraphicEngine;
class MeshRenderer;

class RenderSystem
{
public:
	RenderSystem(GraphicEngine* pGraphic);
	void Rendering();

	void CreateMesh();

private:
	std::array<std::vector<MeshRenderer const*>, 16> m_meshRenderers{};
	GraphicEngine* m_pGraphic;

};

#endif // !RENDER_SYSTEM_INCLUDE__H
