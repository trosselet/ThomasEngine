#ifndef GAME_MANAGER_INCLUDE__H
#define GAME_MANAGER_INCLUDE__H

#include <Windows.h>

class Window;

class GameManager
{
public:
	

	static void Initialize(HINSTANCE hInstance);
	static void Run();
	static void Release();

private:
	GameManager(HINSTANCE hInstance);
	~GameManager();

	void GameLoop();
	void Update();
	void FixedUpdate();

private:
	inline static GameManager* m_pInstance = nullptr;

private:

	Window* m_pWindow = nullptr;
};

#endif // !GAME_MANAGER_INCLUDE__H
