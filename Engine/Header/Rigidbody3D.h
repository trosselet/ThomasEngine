#ifndef RIGIDBODY_3D_INCLUDED__H
#define RIGIDBODY_3D_INCLUDED__H

#include <Engine/Header/ComponentBase.h>

#include <Tools/Header/PrimitiveTypes.h>

#define G GravitationalConstant;

struct Rigidbody3D : ComponentBase<Component::Rigidbody>
{
	float m_mass		= 1;
	float m_drag		= 0;
	float m_angularDrag = 0;
	bool  m_useGravity  = true;

	Vector3 m_velocity	= Vector3{ 0.0f, 0.0f, 0.0f };
};



#endif // !RIGIDBODY_3D_INCLUDED__H
