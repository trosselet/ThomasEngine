#include "pch.h"
#include "Header/main.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    int8 thomas = 8;

    Utils::DebugLog(thomas);
    Utils::DebugWarning("Thomas est un", thomas);
    Utils::DebugError("Le boss est", thomas);

    Window* pWindow = new Window(hInstance);

    while(true)
    {
        pWindow->Update();
    }

    return 0;
}