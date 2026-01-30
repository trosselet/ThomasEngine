#ifndef PHYSICS_SYSTEM_INCLUDED__H
#define PHYSICS_SYSTEM_INCLUDED__H

#include <Engine/Header/GameManager.h>

#include <vector>
struct Rigidbody3D;

class PhysicsSystem
{
public:
	PhysicsSystem() = default;
	~PhysicsSystem();

	void PhysicsUpdate();
private:
	std::vector<Rigidbody3D*> m_rigidbody3d;


private:
	friend class Scene;
	friend class GameManager;
};

#endif // !PHYSICS_SYSTEM_INCLUDED__H
