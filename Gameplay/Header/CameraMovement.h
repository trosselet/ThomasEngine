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
	float mYaw = 0.0f;
	float mPitch = 0.0f;

	std::chrono::steady_clock::time_point lastCameraSwitch;
	float cameraSwitchCooldown = 0.5f;
};


#endif // !CAMERA_MOVEMENT_INCLUDED__H