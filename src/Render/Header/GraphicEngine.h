#ifndef GRAPHIC_ENGINE_INCLUDE__H
#define GRAPHIC_ENGINE_INCLUDE__H

#include "../Tools/Header/Color.h"

class Window;
class Render;

struct Geometry;
enum PrimitiveGeometryType : uint8;

class Mesh;

class GraphicEngine
{
public:
	GraphicEngine(const Window* pWindow);
	~GraphicEngine();

	void BeginDraw();
	void RenderFrame(Mesh* pMesh);
	void Display();

	Geometry* CreatePrimitiveGeometry(PrimitiveGeometryType primitiveType, Color color);
	Mesh* CreateMesh(Geometry* pGeometry);

private:
	Render* m_pRender = nullptr;
};

#endif // !GRAPHIC_ENGINE_INCLUDE__H
