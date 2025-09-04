#include "pch.h"
#include "Header/Scene.h"

#include "Header/GameObject.h"
#include "Header/GameManager.h"
#include "Header/RenderSystem.h"

Scene::~Scene()
{
    for (GameObject* pGameObject : m_gameObjects)
    {
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

std::vector<GameObject*> const& Scene::GetGameObjects() const
{
    return m_gameObjects;
}

std::vector<MeshRenderer*> const& Scene::GetMeshRenderers() const
{
    return m_meshRenderers;
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
    return m_gameObjects;
}

std::vector<MeshRenderer*>& Scene::GetMeshRenderers()
{
    return m_meshRenderers;
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

        pMeshRenderer ->m_toBeCreated = false;
        pMeshRenderer->m_created = true;

        GameManager::GetRenderSystem().m_meshRenderers[pMeshRenderer->renderLayer].push_back(pMeshRenderer);
    }
    m_meshRenderersToCreate.clear();
}

void Scene::HandleDestruction()
{
    for (uint16 i = 0; i < m_meshRenderers.size(); i++)
    {
        MeshRenderer* const pMeshRenderer = m_meshRenderers[i];
        if (pMeshRenderer->m_toBeDeleted == false) continue;

        m_meshRenderers.erase(m_meshRenderers.begin() + i);
        delete pMeshRenderer;
    }

    for (GameObject* const pGameObject : m_gameObjectsToDelete)
    {
        m_gameObjectsIDs.push_back(pGameObject->m_id);
        m_gameObjects.erase(m_gameObjects.begin() + pGameObject->m_id);
        delete pGameObject;
    }
    m_gameObjectsToDelete.clear();

}