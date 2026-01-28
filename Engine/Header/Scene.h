#ifndef SCENE_INCLUDE__H
#define SCENE_INCLUDE__H

#include <Tools/Header/PrimitiveTypes.h>

class GameObject;
struct MeshRenderer;
class GameManager;
class Camera;
struct Rigidbody3D;

class Scene
{
public:
	Scene() = default;
	~Scene();

	static Scene& Create();
	void Load();
	void Unload();
	void SetActive();

	bool IsActive() const;
	bool IsLoaded() const;

	GameObject const* GetMainCamera() const;
	GameObject* GetMainCamera();
	void SetMainCamera(GameObject* const pCamera);

	std::vector<GameObject*> const& GetGameObjects() const;
	std::vector<MeshRenderer*> const& GetMeshRenderers() const;
	std::vector<Rigidbody3D*> const& GetRigidbody3D() const;

	std::vector<GameObject*>& GetGameObjects();
	std::vector<MeshRenderer*>& GetMeshRenderers();
	std::vector<Camera*>& GetCameras();
	std::vector<Rigidbody3D*>& GetRigidbody3D();

	void HandleCreation();
	void HandleDestruction();

private:
	bool m_active = false;
	bool m_loaded = false;
	bool m_toBeLoaded = false;
	bool m_toBeUnloaded = false;

	GameObject* m_pMainCamera = nullptr;

	std::vector<Camera*> m_cameras;
	std::vector<uint32> m_gameObjectsIDs;
	std::vector<GameObject*> m_gameObjects;
	std::vector<Rigidbody3D*> m_rigidbody3d;
	std::vector<MeshRenderer*> m_meshRenderers;

	std::vector<Camera*> m_camerasToCreate;
	std::vector<Rigidbody3D*> m_rigidbody3dToCreate;
	std::vector<GameObject*> m_gameObjectsToCreate;
	std::vector<MeshRenderer*> m_meshRenderersToCreate;

	std::vector<GameObject*> m_gameObjectsToDelete;

	friend struct Camera;
	friend struct Rigidbody3D;
	friend struct MeshRenderer;
	friend class GameObject;
	friend class GameManager;
};

#endif // !SCENE_INCLUDE__H
