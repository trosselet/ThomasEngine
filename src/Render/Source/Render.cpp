#include "pch.h"
#include "Header/Render.h"
#include "Header/RenderResources.h"
#include "Header/Window.h"



Render::Render(const Window* pWindow)
{
	m_pRenderResources = new RenderResources(pWindow->GetHWND(), pWindow->GetWidth(), pWindow->GetHeight());
}

Render::~Render()
{
	delete m_pRenderResources;
}

void Render::Clear()
{
	//Utils::DebugLog("BeginDraw");
}

void Render::Draw()
{
	//Utils::DebugLog("RenderFrame");
}

void Render::Display()
{
	//Utils::DebugLog("Display");
}
