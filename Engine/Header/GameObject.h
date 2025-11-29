#ifndef GAMEOBJECT_INCLUDE__H
#define GAMEOBJECT_INCLUDE__H


#include <Engine/Header/MeshRenderer.h>
#include <Engine/Header/Camera.h>
#include <Engine/Header/Scene.h>
#include <Engine/Header/IScript.h>
#include <Engine/Header/ScriptSystem.h>

#include <Tools/Header/PrimitiveTypes.h>
#include <Tools/Header/Transform.h>

struct Component;
class Color;

class GameObject
{
public:
	GameObject(Scene& scene);
	~GameObject() = default;

	TRANSFORM transform;

	template<class ComponentClass> bool HasComponent() const;
	template<class ComponentClass> ComponentClass const& GetComponent() const;
	template<class ComponentClass> ComponentClass* GetComponent();
	template<class ComponentClass> ComponentClass& AddComponent();
	template<class ComponentClass> void RemoveComponent();

	template<typename T, typename ... Args> T* AddScript(Args&&... args);
	template<typename T> T* GetScript();

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
	std::vector<IScript*> m_scripts;

	uint64 m_componentBitmask = 0;


	friend class Scene;
};

template<typename T, typename ...Args>
inline T* GameObject::AddScript(Args && ...args)
{
	static_assert(std::is_base_of<IScript, T>::value, "T must be derived from IScript");
	T* script = new T(std::forward<Args>(args)...);
	script->SetOwner(this);
	script->OnStart();
	m_scripts.push_back(script);
	GameManager::GetScriptSystem().m_scriptsByEntity[GetId()].push_back(script);
	return script;
}

template<typename T>
inline T* GameObject::GetScript()
{
	for (int i = 0; i < m_scripts.size(); i++)
	{
		if (T* foundScript = dynamic_cast<T*>(m_scripts[i]))
		{
			return foundScript;
		}
	}
	return nullptr;
}




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
inline ComponentClass* GameObject::GetComponent()
{
	return static_cast<ComponentClass*>(m_components[ComponentClass::Tag]);
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

template <>
inline Camera& GameObject::AddComponent<Camera>()
{
	assert((HasComponent<Camera>() == false));

	Camera* const pCamera = new Camera();
	m_pScene->m_cameras.push_back(pCamera);
	pCamera->m_pOwner = this;
	m_pScene->m_camerasToCreate.push_back(pCamera);
	if (m_pScene->m_pMainCamera == nullptr) 
	{
		m_pScene->m_pMainCamera = pCamera->m_pOwner;
	}

	m_components[Camera::Tag] = pCamera;
	m_componentBitmask |= 1 << (Camera::Tag - 1);

	return *pCamera;
}

template <>
inline void GameObject::RemoveComponent<Camera>()
{
	assert(HasComponent<Camera>());
	m_components[Camera::Tag]->Destroy();
}

#endif // !GAMEOBJECT_INCLUDE__H

