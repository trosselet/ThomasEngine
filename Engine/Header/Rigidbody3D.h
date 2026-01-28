#ifndef RIGIDBODY_3D_INCLUDED__H
#define RIGIDBODY_3D_INCLUDED__H

#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/ComponentBase.h>

struct Rigidbody3D : ComponentBase<Component::Rigidbody>
{
public:
	Rigidbody3D();
	~Rigidbody3D();


private:
	float m_mass = 0;
	float m_drag = 0;
	float m_angularDrag = 0;
	bool m_useGravity = true;
};



#endif // !RIGIDBODY_3D_INCLUDED__H
