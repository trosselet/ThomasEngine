#ifndef ISCRIPT__INCLUDE__H
#define ISCRIPT__INCLUDE__H

#include <Engine/Header/GameObject.h>

class IScript
{
public:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();
	virtual void OnDisactivation();

protected:
	GameObject* m_pOwner;

private:
	void SetOwner(GameObject* pOwner) { m_pOwner = pOwner; };

	friend class GameManager;
	friend class GameObject;
};


#endif // !ISCRIPT__INCLUDE__H