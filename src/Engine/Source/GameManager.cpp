#include "pch.h"
#include "Header/GameManager.h"
#include "Header/RenderSystem.h"
#include "Header/Scene.h"
#include "Header/ScriptSystem.h"

#include "Render/Header/Window.h"

GameManager::GameManager(HINSTANCE hInstance) :
	m_pWindow(new Window(hInstance)),
	m_pRenderSystem(new RenderSystem(m_pWindow->GetGraphicEngine())),
	m_pScriptSystem(new ScriptSystem())
{
	
}

GameManager::~GameManager()
{
	for (Scene& scene : m_scenes)
	{
		scene.Unload();
		scene.HandleDestruction(); 
	}

	m_scenes.clear();
	m_loadedScenes.clear();
	m_scenesToLoad.clear();

	delete m_pScriptSystem;
	m_pScriptSystem = nullptr;

	delete m_pRenderSystem;
	m_pRenderSystem = nullptr;

	delete m_pWindow;
	m_pWindow = nullptr;
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
	m_pInstance = nullptr;
}

std::vector<Scene>& GameManager::GetScenes()
{
	return m_pInstance->m_scenes;
}

Scene& GameManager::GetActiveScene()
{
	return *m_pInstance->m_pActiveScene;
}

float32& GameManager::GetFixedDeltaTime()
{
	return m_pInstance->m_fixedDeltaTime;
}

RenderSystem& GameManager::GetRenderSystem()
{
	return *m_pInstance->m_pRenderSystem;
}

ScriptSystem& GameManager::GetScriptSystem()
{
	return *m_pInstance->m_pScriptSystem;
}

void GameManager::GameLoop()
{

	while (m_pWindow->IsOpen())
	{
		Update();

		HandleCreations();
		HandleDeletions();

		FixedUpdate();

		m_pScriptSystem->OnUpdate();

		m_pRenderSystem->Rendering();

	}
}

void GameManager::Update()
{
	m_pWindow->Update();
}

void GameManager::FixedUpdate()
{
	m_pScriptSystem->OnFixedUpdate();
}

void GameManager::HandleCreations()
{
	for (Scene* const pScene : m_scenesToLoad)
	{
		if (pScene->m_toBeUnloaded) continue;

		pScene->m_toBeLoaded = false;
		pScene->m_loaded = true;
		m_loadedScenes.push_back(pScene);
		if (pScene->m_active) m_pNextActiveScene = pScene;
	}
	m_scenesToLoad.clear();

	for (Scene* const pScene : m_loadedScenes)
		pScene->HandleCreation();

	if (m_pNextActiveScene != nullptr)
	{
		m_pActiveScene = m_pNextActiveScene;
		m_pNextActiveScene = nullptr;
	}
}

void GameManager::HandleDeletions()
{
	for (Scene* const pScene : m_loadedScenes)
		pScene->HandleDestruction();

	for (uint8 i = 0; i < m_loadedScenes.size(); i++)
	{
		Scene& scene = *m_loadedScenes[i];
		if (scene.m_toBeUnloaded == false) continue;

		if (scene.m_toBeLoaded)
		{
			scene.m_toBeLoaded = false;
			scene.m_toBeUnloaded = false;
			continue;
		}

		scene.m_toBeUnloaded = false;
		scene.m_loaded = false;
		scene.m_gameObjects.clear();
		m_loadedScenes.erase(m_loadedScenes.begin() + i--);
	}
}
