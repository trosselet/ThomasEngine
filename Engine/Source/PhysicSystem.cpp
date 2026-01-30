#include <Engine/pch.h>
#include <Engine/Header/PhysicsSystem.h>
#include <Engine/Header/Rigidbody3D.h>
#include <Engine/Header/GameObject.h>
#include <Engine/Header/GameManager.h>


PhysicsSystem::~PhysicsSystem()
{
	m_rigidbody3d.clear();
}

void PhysicsSystem::PhysicsUpdate()
{
	for (int i = 0; i < static_cast<int>(m_rigidbody3d.size()); i++)
	{
		float mass = m_rigidbody3d[i]->m_mass;
		Vector3 gravity = {0.0f, 0.0f, 0.0f};

		if (m_rigidbody3d[i]->m_useGravity)
		{
			gravity = G;
		}

		Vector3 sumOfForces = Vector3(m_rigidbody3d[i]->m_drag, m_rigidbody3d[i]->m_drag, m_rigidbody3d[i]->m_drag) - gravity;

		float ax = (sumOfForces.data[0] / mass);
		float ay = (sumOfForces.data[1] / mass);
		float az = (sumOfForces.data[2] / mass);

		Vector3 velocity =
		{
			m_rigidbody3d[i]->m_velocity.data[0] + ax * GameManager::GetDeltaTime(),
			m_rigidbody3d[i]->m_velocity.data[1] + ay * GameManager::GetDeltaTime(),
			m_rigidbody3d[i]->m_velocity.data[2] + az * GameManager::GetDeltaTime(),
		};

		m_rigidbody3d[i]->GetOwner().transform.OffsetPosition(velocity.ToXMFLOAT3());

		Sleep(0);

	}
}