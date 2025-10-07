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
    pRectangle->SetName("Rectangle1");

    GameObject* const pSphere1 = new GameObject(scene);
    pSphere1->transform.SetPosition({ 1.5f, 0.0f, 5.0f });
    MeshRenderer& pSphere1MeshRendererTest = pSphere1->AddComponent<MeshRenderer>();
    pSphere1MeshRendererTest.SetSphere();
    pSphere1->SetName("Sphere1");
   
    GameObject* const pCircle1 = new GameObject(scene);
    pCircle1->transform.SetPosition({ 3.0f, 0.0f, 5.0f });
    MeshRenderer& pCircle1MeshRendererTest = pCircle1->AddComponent<MeshRenderer>();
    pCircle1MeshRendererTest.SetCircle();
    pCircle1->SetName("Circle1");
   
    GameObject* const pCube1 = new GameObject(scene);
    pCube1->transform.SetPosition({ -1.5f, 0.0f, 5.0f });
    MeshRenderer& pCube1MeshRendererTest = pCube1->AddComponent<MeshRenderer>();
    pCube1MeshRendererTest.SetCube();
    pCube1->SetName("Cube");

    GameObject* const pSky = new GameObject(scene);
    pSky->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    pSky->transform.SetScaling(Vector3{ -10000.0f, -10000.0f, -10000.0f });
    MeshRenderer& pSkyMeshRendererTest = pSky->AddComponent<MeshRenderer>();
    pSkyMeshRendererTest.SetSphere("sky.dds");
    pSky->SetName("Sky");

    pCube1->SetColor(Color::Transparent);

    GameManager::Run();
    GameManager::Release();


    return 0;
}