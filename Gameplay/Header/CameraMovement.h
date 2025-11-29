#ifndef CAMERA_MOVEMENT_INCLUDED__H
#define CAMERA_MOVEMENT_INCLUDED__H

class IScript;

class CameraMovement : public IScript
{
public:
	CameraMovement();
	~CameraMovement();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;

private:
	float cameraSpeed;
};


#endif // !CAMERA_MOVEMENT_INCLUDED__H