#ifndef GAMEOBJECT_INCLUDE__H
#define GAMEOBJECT_INCLUDE__H

#include "../Tools/Header/PrimitiveTypes.h"

#include "Engine/Header/MeshRenderer.h"
#include "Engine/Header/Scene.h"

struct Component;

class GameObject
{
public:
	GameObject(Scene& scene);
	~GameObject() = default;

	template<class ComponentClass> bool HasComponent() const;
	template<class ComponentClass> ComponentClass const& GetComponent() const;
	template<class ComponentClass> ComponentClass& GetComponent();
	template<class ComponentClass> ComponentClass& AddComponent();
	template<class ComponentClass> void RemoveComponent();

	bool IsActive();
	void SetActive(bool active);
	void Activate();
	void Deactivate();

	void Destroy();

	uint32 GetId() const;
	int8 const* GetName() const;
	GameObject const* GetParent() const;
	std::vector<GameObject*> const& GetChildren() const;
	
	Scene& GetScene();
	GameObject* GetParent();
	std::vector<GameObject*>& GetChildren();

	void SetName(int8 const* name);

private:
	bool m_created = false;
	bool m_toBeCreated = true;
	bool m_toBeDestroy = false;
	bool m_active = true;

	uint32 m_id = 0;
	int8 const* m_pName = nullptr;

	Scene* m_pScene = nullptr;

	GameObject* m_pParent = nullptr;
	std::vector<GameObject*> m_pChildren;
	std::unordered_map<uint16, Component*> m_components;

	uint64 m_componentBitmask = 0;


	friend class Scene;
};

#endif // !GAMEOBJECT_INCLUDE__H

template<class ComponentClass>
inline bool GameObject::HasComponent() const
{
	return m_componentBitmask & (1 << (ComponentClass::Tag - 1));
}

template<class ComponentClass>
inline ComponentClass const& GameObject::GetComponent() const
{
	return *static_cast<ComponentClass const*>(m_components[ComponentClass::Tag]);
}

template<class ComponentClass>
inline ComponentClass& GameObject::GetComponent()
{
	return *static_cast<ComponentClass*>(m_components[ComponentClass::Tag]);
}

template <>
inline MeshRenderer& GameObject::AddComponent<MeshRenderer>()
{
	assert(HasComponent<MeshRenderer>() == false);

	MeshRenderer* const pMeshRenderer = new MeshRenderer();
	m_pScene->m_meshRenderers.push_back(pMeshRenderer);
	pMeshRenderer->m_pOwner = this;
	m_pScene->m_meshRenderersToCreate.push_back(pMeshRenderer);

	m_components[MeshRenderer::Tag] = pMeshRenderer;
	m_componentBitmask |= 1 << (MeshRenderer::Tag - 1);

	return *pMeshRenderer;
}

template <>
inline void GameObject::RemoveComponent<MeshRenderer>()
{
	assert(!HasComponent<MeshRenderer>());
	m_components[MeshRenderer::Tag]->Destroy();
}