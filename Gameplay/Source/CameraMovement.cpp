#include <Gameplay/pch.h>
#include <Gameplay/Header/CameraMovement.h>
#include <Engine/Header/IScript.h>
#include <Engine/Header/GameManager.h>

CameraMovement::CameraMovement()
{
    cameraSpeed = 0.05f;
}

CameraMovement::~CameraMovement()
{
}

void CameraMovement::OnStart()
{
}

void CameraMovement::OnUpdate()
{
    
}

void CameraMovement::OnFixedUpdate()
{
    bool Z = GetAsyncKeyState('Z') & 0x8000;
    bool Q = GetAsyncKeyState('Q') & 0x8000;
    bool S = GetAsyncKeyState('S') & 0x8000;
    bool D = GetAsyncKeyState('D') & 0x8000;
    bool A = GetAsyncKeyState('A') & 0x8000;
    bool E = GetAsyncKeyState('E') & 0x8000;

    bool LEFT = GetAsyncKeyState(VK_LEFT) & 0x8000;
    bool RIGHT = GetAsyncKeyState(VK_RIGHT) & 0x8000;
    bool UP = GetAsyncKeyState(VK_UP) & 0x8000;
    bool DOWN = GetAsyncKeyState(VK_DOWN) & 0x8000;

    TRANSFORM& t = m_pOwner->transform;

    Vector3 f = t.Forward();
    Vector3 r = t.Right();
    Vector3 u = t.Up();

    const float moveSpeed = cameraSpeed;
    const float rotSpeed = 0.02f;


    if (Z) t.OffsetPosition((f * moveSpeed).ToXMFLOAT3());
    if (S) t.OffsetPosition((f * -moveSpeed).ToXMFLOAT3());
    if (D) t.OffsetPosition((r * moveSpeed).ToXMFLOAT3());
    if (Q) t.OffsetPosition((r * -moveSpeed).ToXMFLOAT3());
    if (E) t.OffsetPosition((u * moveSpeed).ToXMFLOAT3());
    if (A) t.OffsetPosition((u * -moveSpeed).ToXMFLOAT3());


    float pitch = 0;
    float yaw = 0;

    if (LEFT)  yaw -= rotSpeed;
    if (RIGHT) yaw += rotSpeed;
    if (UP)    pitch -= rotSpeed;
    if (DOWN)  pitch += rotSpeed;

    if (yaw != 0)
        t.RotateYaw(yaw);

    if (pitch != 0)
        t.RotatePitch(pitch);
}