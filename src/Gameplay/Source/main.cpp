#define _CRTDBG_MAP_ALLOC

#include "pch.h"
#include "Header/main.h"

#include "Header/CameraMovement.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    GameManager::Initialize(hInstance);

    Scene& scene = Scene::Create();

    scene.Load();
    scene.SetActive();

    GameObject* const pMainCamera = new GameObject(scene);
    pMainCamera->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    Camera& cameraComponent = pMainCamera->AddComponent<Camera>();
    pMainCamera->SetName("Camera");
    pMainCamera->AddScript<CameraMovement>();

    GameObject* const pRectangle = new GameObject(scene);
    pRectangle->transform.SetPosition({ 0.0f, 0.0f, 5.0f });
    MeshRenderer& pRectangleMeshRendererTest = pRectangle->AddComponent<MeshRenderer>();
    pRectangleMeshRendererTest.SetRectangle();
    pRectangle->SetName("Rectangle");

    GameObject* const pRectangle1 = new GameObject(scene);
    pRectangle1->transform.SetPosition({ 1.5f, 0.0f, 5.0f });
    MeshRenderer& pRectangle1MeshRendererTest = pRectangle1->AddComponent<MeshRenderer>();
    pRectangle1MeshRendererTest.SetRectangle();
    pRectangle1->SetName("Rectangle2");

    GameManager::Run();
    GameManager::Release();


    return 0;
}