#include "pch.h"
#include "Header/GameManager.h"
#include "Header/RenderSystem.h"

#include "Render/Header/Window.h"

GameManager::GameManager(HINSTANCE hInstance)
{
	m_pWindow = new Window(hInstance, 1200, 800, L"EngineWindow");
	if (m_pWindow != nullptr) m_pRenderSystem = new RenderSystem(m_pWindow->GetGraphicEngine());

	
}

GameManager::~GameManager()
{
	delete m_pRenderSystem;
	delete m_pWindow;

}

void GameManager::Initialize(HINSTANCE hInstance)
{
	if (m_pInstance != nullptr)
	{
		Utils::DebugError("A game manager instance already exist !");
		return;
	}

	m_pInstance = new GameManager(hInstance);
}

void GameManager::Run()
{
	m_pInstance->GameLoop();
}

void GameManager::Release()
{
	if (m_pInstance == nullptr)
	{
		Utils::DebugError("A game manager instance has already been destroyed !");
		return;
	}

	delete m_pInstance;
}

void GameManager::GameLoop()
{
	m_pRenderSystem->CreateMesh();

	while (m_pWindow->IsOpen())
	{
		Update();
		FixedUpdate();

		m_pRenderSystem->Rendering();

	}
}

void GameManager::Update()
{
	m_pWindow->Update();
}

void GameManager::FixedUpdate()
{
}