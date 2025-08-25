#include "pch.h"
#include "Header/GameManager.h"

#include "Render/Header/Window.h"

GameManager::GameManager(HINSTANCE hInstance)
{
	m_pWindow = new Window(hInstance, 1200, 800, L"EngineWindow");
}

GameManager::~GameManager()
{
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
	while (m_pWindow->IsOpen())
	{
		Update();
		FixedUpdate();
	}
}

void GameManager::Update()
{
	m_pWindow->Update();
}

void GameManager::FixedUpdate()
{
}