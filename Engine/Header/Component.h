#ifndef COMPONENT_INCLUDE__H
#define COMPONENT_INCLUDE__H

#include <Tools/Header/PrimitiveTypes.h>

class GameObject;

struct Component
{
	enum Tag : uint16
	{
		None,
		MeshRenderer,
		BoxCollider,
		SphereCollider,
		Rigidbody,
		CharacterController,
		Camera,
		Light,
	};

	void Destroy();
	bool IsActive() const;
	void SetActive(bool active);
	void Activate();
	void Deactivate();

	GameObject const& GetOwner() const;

	GameObject& GetOwner();

protected:
	Component() = default;
	virtual ~Component() = default;

	bool m_active = true;
	bool m_created = false;
	bool m_toBeCreated = true;
	bool m_toBeDeleted = false;

	GameObject* m_pOwner = nullptr;

	friend class GameObject;
	friend class Scene;
};



#endif // !COMPONENT_INCLUDE__H
