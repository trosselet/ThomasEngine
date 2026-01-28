#include <Engine/pch.h>

#include <Engine/Header/Scene.h>
#include <Engine/Header/GameObject.h>
#include <Engine/Header/GameManager.h>
#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/Camera.h>
#include <Engine/Header/Rigidbody3D.h>

#include <Render/Header/Window.h>
#include <Render/Header/GraphicEngine.h>

Scene::~Scene()
{
    for (GameObject* pGameObject : m_gameObjects)
    {
		if (pGameObject != nullptr)
            pGameObject->Destroy();
    }

    HandleDestruction();
}

Scene& Scene::Create()
{
    std::vector<Scene>& scenes = GameManager::GetScenes();
    scenes.push_back(Scene());
    return scenes.back();
}

void Scene::Load()
{
    if (m_toBeLoaded || m_loaded) return;

    m_toBeLoaded = true;

    GameManager::m_pInstance->m_scenesToLoad.push_back(this);
}

void Scene::Unload()
{
    if (m_toBeUnloaded || IsActive()) return;

    m_toBeUnloaded = true;

    for (GameObject* const gameObject : m_gameObjects)
    {
        gameObject->Destroy();
    }
}

void Scene::SetActive()
{
    GameManager& gameManager = *GameManager::m_pInstance;
    if (gameManager.m_pActiveScene == this || gameManager.m_pNextActiveScene) return;

    if (m_loaded = false)
    {
        m_active = true;
        return;
    }
    gameManager.m_pNextActiveScene = this;
}

bool Scene::IsActive() const
{
    return m_active;
}

bool Scene::IsLoaded() const
{
    return m_loaded;
}

GameObject const* Scene::GetMainCamera() const
{
    return m_pMainCamera;
}

GameObject* Scene::GetMainCamera()
{
    return m_pMainCamera;
}

void Scene::SetMainCamera(GameObject* const pCamera)
{
	m_pMainCamera = pCamera;
}

std::vector<GameObject*> const& Scene::GetGameObjects() const
{
    return m_gameObjects;
}

std::vector<MeshRenderer*> const& Scene::GetMeshRenderers() const
{
    return m_meshRenderers;
}

std::vector<Rigidbody3D*> const& Scene::GetRigidbody3D() const
{
    return m_rigidbody3d;
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
    return m_gameObjects;
}

std::vector<MeshRenderer*>& Scene::GetMeshRenderers()
{
    return m_meshRenderers;
}

std::vector<Camera*>& Scene::GetCameras()
{
    return m_cameras;
}

std::vector<Rigidbody3D*>& Scene::GetRigidbody3D()
{
    return m_rigidbody3d;
}

void Scene::HandleCreation()
{
    for (GameObject* const pGameObject : m_gameObjectsToCreate)
    {

        pGameObject->m_toBeCreated = false;
        pGameObject->m_created = true;
    }
    m_gameObjectsToCreate.clear();

    for (MeshRenderer* const pMeshRenderer : m_meshRenderersToCreate)
    {

        pMeshRenderer->m_toBeCreated = false;
        pMeshRenderer->m_created = true;

        GameManager::GetRenderSystem().m_meshRenderers[pMeshRenderer->renderLayer].push_back(pMeshRenderer);
    }
    m_meshRenderersToCreate.clear();

    GameManager::GetWindow().GetGraphicEngine()->ProcessPendingUploads();

    for (Camera* const pCamera : m_camerasToCreate)
    {

        pCamera->m_toBeCreated = false;
        pCamera->m_created = true;
    }
    m_camerasToCreate.clear();

    for (Rigidbody3D* const pRigidbody3D : m_rigidbody3d)
    {
        pRigidbody3D->m_toBeCreated = false;
        pRigidbody3D->m_created = true;
    }
    m_rigidbody3dToCreate.clear();
}

void Scene::HandleDestruction()
{
    for (int i = static_cast<int>(m_rigidbody3d.size()) - 1; i >= 0; i--)
    {
        Rigidbody3D* const pRigidbody3d = m_rigidbody3d[i];
        if (!pRigidbody3d->m_toBeDeleted)
            continue;

        delete pRigidbody3d;
        m_rigidbody3d.erase(m_rigidbody3d.begin() + i);
    }
    
    for (int i = static_cast<int>(m_cameras.size()) - 1; i >= 0; i--)
    {
        Camera* const pCamera = m_cameras[i];
        if (!pCamera->m_toBeDeleted) 
            continue;

        delete pCamera;
        m_cameras.erase(m_cameras.begin() + i);
    }


    for (GameObject* const pGameObject : m_gameObjectsToDelete)
    {
        m_gameObjects[pGameObject->m_id] = nullptr;    
        m_gameObjectsIDs.push_back(pGameObject->m_id); 
        delete pGameObject;                            
    }
    m_gameObjectsToDelete.clear();


    for (int i = static_cast<int>(m_meshRenderers.size()) - 1; i >= 0; i--)
    {
        MeshRenderer* const pMeshRenderer = m_meshRenderers[i];
        if (!pMeshRenderer->m_toBeDeleted)
            continue;

        delete pMeshRenderer;
        m_meshRenderers.erase(m_meshRenderers.begin() + i);
    }

}