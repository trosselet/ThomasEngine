#include "pch.h"
#include "Header/GraphicEngine.h"

#include "Header/Window.h"
#include "Header/Render.h"

GraphicEngine::GraphicEngine(const Window* pWindow)
{
	m_pRender = new Render(pWindow);
}

GraphicEngine::~GraphicEngine()
{
	delete m_pRender;
}

void GraphicEngine::BeginDraw()
{
	m_pRender->Clear();
}

void GraphicEngine::RenderFrame()
{
	m_pRender->Draw();
}

void GraphicEngine::Display()
{
	m_pRender->Display();
}
