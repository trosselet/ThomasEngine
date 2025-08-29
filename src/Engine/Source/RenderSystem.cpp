#include "pch.h"
#include "Header/RenderSystem.h"

#include "../Render/Header/GraphicEngine.h"

RenderSystem::RenderSystem(GraphicEngine* pGraphic)
{
	m_pGraphic = pGraphic;
}

void RenderSystem::Rendering()
{
	m_pGraphic->BeginDraw();
	m_pGraphic->RenderFrame();
	m_pGraphic->Display();
}
