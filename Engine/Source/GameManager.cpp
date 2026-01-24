#include <Engine/pch.h>
#include <Engine/Header/GameManager.h>
#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/Scene.h>
#include <Engine/Header/ScriptSystem.h>

#include <Render/Header/Window.h>
#include <Render/Header/GraphicEngine.h>

GameManager::GameManager(HINSTANCE hInstance) :
	m_pWindow(NEW Window(hInstance)),
	m_pRenderSystem(NEW RenderSystem(m_pWindow->GetGraphicEngine())),
	m_pScriptSystem(NEW ScriptSystem())
{
	m_lastTime = std::chrono::steady_clock::now();
}

GameManager::~GameManager()
{
	m_renderThreadRunning = false;
	if (m_renderThread.joinable())
	{
		m_renderThread.join();
	}

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

	m_pInstance = NEW GameManager(hInstance);
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

float32& GameManager::GetDeltaTime()
{
	return m_pInstance->m_deltaTime;
}

RenderSystem& GameManager::GetRenderSystem()
{
	return *m_pInstance->m_pRenderSystem;
}

ScriptSystem& GameManager::GetScriptSystem()
{
	return *m_pInstance->m_pScriptSystem;
}

void GameManager::SetWireframe(bool wireframe)
{
	m_pInstance->m_pWindow->GetGraphicEngine()->SetWireframe(wireframe);
}

void GameManager::GameLoop()
{
	float m_secondCounter = 0;
	float m_tempFps = 0;
	float fps = 0;

	while (m_pWindow->IsOpen())
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<float> frameTime = now - m_lastTime;
		m_lastTime = now;

		m_deltaTime = frameTime.count();

		if (m_secondCounter <= 1) {
			m_secondCounter += m_deltaTime;
			m_tempFps++;
		}
		else
		{ 
			fps = m_tempFps;
			m_secondCounter = 0;
			m_tempFps = 0;
		}

		printf("\033[%d;%dH\033[2K  [ENGINE] Frame Time: %d fps", 1, 0, static_cast<int>(fps));
		printf("\033[%d;%dH\033[2K  [ENGINE] Game Loop: %f", 12, 0, m_deltaTime);

		m_accumulator += m_deltaTime;

		////////////
		// Update //
		////////////

		float updateStartTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		Update();
		float updateEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		printf("\033[%d;%dH\033[2K  [ENGINE] Update time: %f", 2, 0, updateEndTime - updateStartTime);

		//////////////////////
		// Handle Creations //
		//////////////////////

		float handleCreationStartTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		HandleCreations();
		float handleCreationEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		printf("\033[%d;%dH\033[2K  [ENGINE] Handle Creation time: %f", 3, 0, handleCreationEndTime - handleCreationStartTime);

		//////////////////////
		// Handle Deletions //
		//////////////////////

		float handleDeletionStartTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		HandleDeletions();
		float handleDeletionEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		printf("\033[%d;%dH\033[2K  [ENGINE] Handle Deletion time: %f", 4, 0, handleDeletionEndTime - handleDeletionStartTime);

		//////////////////
		// Fixed Update //
		//////////////////

		float fixedUpdateStartTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		while (m_accumulator >= m_fixedDeltaTime)
		{
			FixedUpdate();
			m_accumulator -= m_fixedDeltaTime;
		}
		float fixedUpdateEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		printf("\033[%d;%dH\033[2K  [ENGINE] FixedUpdate time: %f", 5, 0, fixedUpdateEndTime - fixedUpdateStartTime);

		///////////////////
		// Script Update //
		///////////////////

		float scriptUpdateStartTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		m_pScriptSystem->OnUpdate();
		float scriptUpdateEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()) / 1'000'000.0f;
		printf("\033[%d;%dH\033[2K  [ENGINE] Script Update time: %f", 6, 0, scriptUpdateEndTime - scriptUpdateStartTime);

		///////////////
		// Rendering //
		///////////////

		if (!m_renderThread.joinable())
		{
			m_renderThread = std::thread(&GameManager::RenderThreadFunc, this);
		}

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

void GameManager::RenderThreadFunc()
{
	m_renderThreadRunning = true;

	while (m_pWindow->IsOpen() && m_renderThreadRunning)
	{
		auto startTime = std::chrono::steady_clock::now();

		m_pRenderSystem->Rendering();

		float renderEndTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()) / 1'000'000.0f;

		printf("\033[%d;%dH\033[2K  [RENDER] Rendering time: %f", 10, 0, renderEndTime);
		printf("\033[%d;%dH\033[2K  [RENDER] Scene object number: %d", 11, 0, (int)GetActiveScene().GetGameObjects().size());

		//std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}
}