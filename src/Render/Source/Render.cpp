#include "pch.h"
#include "Header/Render.h"
#include "Header/RenderResources.h"

Render::Render(const Window* pWindow)
{
	m_pRenderResources = new RenderResources();
}

Render::~Render()
{
	delete m_pRenderResources;
}

void Render::Clear()
{
}

void Render::Draw()
{
}

void Render::Display()
{
}
