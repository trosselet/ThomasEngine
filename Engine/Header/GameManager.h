#ifndef GAME_MANAGER_INCLUDE__H
#define GAME_MANAGER_INCLUDE__H

#include <Windows.h>
#include <Tools/Header/PrimitiveTypes.h>
#include <chrono>

class Window;
class RenderSystem;
class Scene;
class ScriptSystem;

class GameManager
{
public:


	static void Initialize(HINSTANCE hInstance);
	static void Run();
	static void Release();

	static Window& GetWindow() { return *m_pInstance->m_pWindow; };

	static std::vector<Scene>& GetScenes();
	static Scene& GetActiveScene();
	static float32& GetFixedDeltaTime();
	static float32& GetDeltaTime();

	static RenderSystem& GetRenderSystem();
	static ScriptSystem& GetScriptSystem();



private:
	GameManager(HINSTANCE hInstance);
	~GameManager();

	void GameLoop();
	void Update();
	void FixedUpdate();

	void HandleCreations();
	void HandleDeletions();

private:
	inline static GameManager* m_pInstance = nullptr;

private:

	Window* m_pWindow = nullptr;
	RenderSystem* m_pRenderSystem = nullptr;
	ScriptSystem* m_pScriptSystem = nullptr;

	std::vector<Scene>  m_scenes;
	std::vector<Scene*> m_loadedScenes;
	Scene* m_pActiveScene = nullptr;
	Scene* m_pNextActiveScene = nullptr;
	std::vector<Scene*> m_scenesToLoad;

	float32 m_fixedDeltaTime = 0.016f;
	float32 m_elapsedTime = 0.0f;
	float32 m_deltaTime = 0.0f;
	float32 m_accumulator = 0.0f;
	std::chrono::steady_clock::time_point m_lastTime;

	friend class GameObject;
	friend class Scene;
};

#endif // !GAME_MANAGER_INCLUDE__H