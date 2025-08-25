#define _CRTDBG_MAP_ALLOC

#include "pch.h"
#include "Header/main.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    GameManager::Initialize(hInstance);
    GameManager::Run();
    GameManager::Release();


    return 0;
}