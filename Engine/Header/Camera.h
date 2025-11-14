#ifndef CAMERA_INCLUDE__H
#define CAMERA_INCLUDE__H

#include <Engine/Header/ComponentBase.h>

#include <Tools/Header/PrimitiveTypes.h>

struct Camera : public ComponentBase<Component::Camera>
{
	float32 viewWidth = 1920;
	float32 viewHeight = 1080;

	float32 fov = 0.25 * PI;

	float32 nearZ = 0.1f;
	float32 farZ = 10000000.0f;

	Matri4x4 projectionMatrix;
	Matri4x4 viewMatrix;
};


#endif // !CAMERA_INCLUDE__H
