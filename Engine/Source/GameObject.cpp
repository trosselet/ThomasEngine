#include <Engine/pch.h>
#include <Engine/Header/GameObject.h>

#include <Tools/Header/Color.h>
#include <Render/Header/Geometry.h>

GameObject::GameObject(Scene& scene):
    m_pScene(&scene)
{

    if (m_pScene->m_gameObjectsIDs.empty())
    {
        m_id = (uint32)m_pScene->m_gameObjectsIDs.size();
        m_pScene->m_gameObjects.push_back(this);
    }
    else
    {
        m_id = m_pScene->m_gameObjectsIDs.back();
        m_pScene->m_gameObjectsIDs.pop_back();
        m_pScene->m_gameObjects[m_id] = this;
    }
    m_pScene->m_gameObjectsToCreate.push_back(this);

}

bool GameObject::IsActive()
{
    return m_active;
}

void GameObject::SetActive(bool active)
{
    m_active = active;
}

void GameObject::Activate()
{
    m_active = true;
}

void GameObject::Deactivate()
{
    m_active = false;
}

void GameObject::Destroy()
{
    if (m_toBeDestroy) return;

    m_toBeDestroy = true;

    for (GameObject* const pChild : m_pChildren)
        pChild->Destroy();

    for (auto const& [tag, pComponent] : m_components)
    {
		if (pComponent != nullptr)
            pComponent->Destroy();
    }

    GameManager::GetActiveScene().m_gameObjectsToDelete.push_back(this);
}

uint32 GameObject::GetId() const
{
    return m_id;
}

int8 const* GameObject::GetName() const
{
    return m_pName;
}

GameObject const* GameObject::GetParent() const
{
    return m_pParent;
}

std::vector<GameObject*> const& GameObject::GetChildren() const
{
    return m_pChildren;
}

Scene& GameObject::GetScene()
{
    return *m_pScene;
}

GameObject* GameObject::GetParent()
{
    return m_pParent;
}

std::vector<GameObject*>& GameObject::GetChildren()
{
    return m_pChildren;
}

void GameObject::SetName(int8 const* name)
{
    m_pName = name;
}