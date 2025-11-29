#include <Gameplay/pch.h>
#include <Gameplay/Header/CameraMovement.h>
#include <Engine/Header/IScript.h>
#include <Engine/Header/GameManager.h>
#include <Engine/Header/MeshRenderer.h>

constexpr float PIDIV2 = 1.570796327f;

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
    bool M = GetAsyncKeyState('M') & 0x8000;

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

    // Déplacement
    if (Z) t.OffsetPosition((f * moveSpeed).ToXMFLOAT3());
    if (S) t.OffsetPosition((f * -moveSpeed).ToXMFLOAT3());
    if (D) t.OffsetPosition((r * moveSpeed).ToXMFLOAT3());
    if (Q) t.OffsetPosition((r * -moveSpeed).ToXMFLOAT3());
    if (E) t.OffsetPosition((u * moveSpeed).ToXMFLOAT3());
    if (A) t.OffsetPosition((u * -moveSpeed).ToXMFLOAT3());

    if (LEFT)  mYaw -= rotSpeed;
    if (RIGHT) mYaw += rotSpeed;
    if (UP)    mPitch -= rotSpeed;
    if (DOWN)  mPitch += rotSpeed;

    const float limit = PIDIV2 - 0.01f;
    mPitch = std::clamp(mPitch, -limit, limit);

    t.SetCameraRotation(mYaw, mPitch);
}