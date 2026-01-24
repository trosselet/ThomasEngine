#define _CRTDBG_MAP_ALLOC

#include <Gameplay/pch.h>

#include <Gameplay/Header/main.h>
#include <Gameplay/Header/CameraMovement.h>


int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    GameManager::Initialize(hInstance);

    Scene& scene = Scene::Create();

    scene.Load();
    scene.SetActive();

    GameObject* const pMainCamera = NEW GameObject(scene);
    pMainCamera->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    Camera& cameraComponent = pMainCamera->AddComponent<Camera>();
    pMainCamera->SetName("Camera");
    pMainCamera->AddScript<CameraMovement>();
    
    GameObject* const pSecondCamera = NEW GameObject(scene);
    pSecondCamera->transform.SetPosition({ 0.0f, 0.0f, 2.0f });
    Camera& secondCameraComponent = pSecondCamera->AddComponent<Camera>();
    MeshRenderer& pCameraMeshRenderer = pSecondCamera->AddComponent<MeshRenderer>();
    pCameraMeshRenderer.SetSphere(Color::White);
	pCameraMeshRenderer.SetTexture("wood.jpg");
    pSecondCamera->SetName("Camera2");
    pSecondCamera->AddScript<CameraMovement>();

    GameObject* const pPlateform = NEW GameObject(scene);
    pPlateform->transform.SetPosition({ 0.0f, -1.0f, 5.0f });
    pPlateform->transform.SetScaling(Vector3{ 10.0f, 0.2f, 10.0f });
    MeshRenderer& pPlateformMeshRenderer = pPlateform->AddComponent<MeshRenderer>();
    pPlateformMeshRenderer.SetCube("grid_placeholder_material.png", Color::White, static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite));
    pPlateform->SetName("Plateform");

    GameObject* const pRectangle = NEW GameObject(scene);
    pRectangle->transform.SetPosition({ 0.0f, 0.0f, 5.0f });
    MeshRenderer& pRectangleMeshRendererTest = pRectangle->AddComponent<MeshRenderer>();
    pRectangleMeshRendererTest.SetRectangle(Color::White, static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite));
    pRectangle->SetName("Rectangle1");

    /*for (int i = 0; i < 8000; i++)
    {
        GameObject* const pSphere1 = NEW GameObject(scene);
        pSphere1->transform.SetPosition({ 1.5f, 0.0f, 5.0f });
        MeshRenderer& pSphere1MeshRendererTest = pSphere1->AddComponent<MeshRenderer>();
        pSphere1MeshRendererTest.SetSphere(Color::White);
        pSphere1->SetName("Sphere1");
    }*/
   
    /*GameObject* const pCircle1 = NEW GameObject(scene);
    pCircle1->transform.SetPosition({ 3.0f, 0.0f, 5.0f });
    MeshRenderer& pCircle1MeshRendererTest = pCircle1->AddComponent<MeshRenderer>();
    pCircle1MeshRendererTest.SetCircle(Color::White, static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite));
    pCircle1MeshRendererTest.SetColor(Color::Blue);
    pCircle1->SetName("Circle1");*/
   
    /*GameObject* const pCube1 = NEW GameObject(scene);
    pCube1->transform.SetPosition({ -1.5f, 0.0f, 5.0f });
    MeshRenderer& pCube1MeshRendererTest = pCube1->AddComponent<MeshRenderer>();
    pCube1MeshRendererTest.SetCube("wood.jpg", Color::White, static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite));
    pCube1->SetName("Cube");*/

    GameObject* const pSky = NEW GameObject(scene);
    pSky->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    pSky->transform.RotateCartesian({0.0f, 0.0f, 180.0f});
    pSky->transform.SetScaling(Vector3{ 10000.0f, 10000.0f, 10000.0f });
    MeshRenderer& pSkyMeshRendererTest = pSky->AddComponent<MeshRenderer>();
    pSkyMeshRendererTest.SetSphere("sky.jpg", Color::White, static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite | Utils::PSOFlags::CullFront));
    pSky->SetName("Sky");

    /*GameObject* const pObj = NEW GameObject(scene);
    pObj->transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    pObj->transform.SetScaling(Vector3{ 0.1f, 0.1f, 0.1f });
    MeshRenderer& pObjMeshRendererTest = pObj->AddComponent<MeshRenderer>();
    pObjMeshRendererTest.SetMeshFile("Female/Female.obj", static_cast<uint32>(Utils::PSOFlags::DepthEnable | Utils::PSOFlags::DepthWrite));
    pObj->SetName("Obj");*/

    for (int i = 0; i < 10000; i+=5)
    {
        GameObject* const pObj1 = NEW GameObject(scene);
        pObj1->transform.SetPosition({ i + 0.0f, 0.0f, 0.0f });
        pObj1->transform.RotateCartesian({ 0.0f, 0.0f, 0.0f });
        pObj1->transform.SetScaling(Vector3{ 0.01f, 0.01f, 0.01f });
        MeshRenderer& pObj1MeshRendererTest = pObj1->AddComponent<MeshRenderer>();
        pObj1MeshRendererTest.SetMeshFile("Dying.fbx");
        pObj1->SetName("Obj2");
    }

    GameManager::Run();
    GameManager::Release();


    return 0;
}