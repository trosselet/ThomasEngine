#ifndef TRANSFORM_INCLUDE__H
#define TRANSFORM_INCLUDE__H

#include <DirectXMath.h>
#include <vector>

#include <Tools/Header/PrimitiveTypes.h>

struct TRANSFORM
{
public:

    DirectX::XMFLOAT3 mvPosition;
    DirectX::XMFLOAT3 mvScaling;

    DirectX::XMFLOAT3 mvRight;
    DirectX::XMFLOAT3 mvUp;
    DirectX::XMFLOAT3 mvForward;
    DirectX::XMFLOAT3 mvOrigin;

    DirectX::XMFLOAT4 mqRotation;
    DirectX::XMFLOAT4X4 mmRotation;

    DirectX::XMFLOAT4X4 mmMatrix;
    bool mIsUpdated = true;

    void UpdateMatrix();
public:
    TRANSFORM();
    /*TRANSFORM(
        const DirectX::XMFLOAT3& _position,
        const DirectX::XMFLOAT3& _rotationYawPitchRoll,
        const DirectX::XMFLOAT3& _scaling
    );*/

    const DirectX::XMFLOAT3& GetPositionFLOAT() const;
    DirectX::XMVECTOR GetPosition();
    void SetPosition(DirectX::XMFLOAT3& _vec);
    void XM_CALLCONV SetPosition(DirectX::FXMVECTOR _vec);
    void OffsetPosition(const DirectX::XMFLOAT3& _vec);

    const DirectX::XMFLOAT4& GetRotationFLOAT() const;
    DirectX::XMVECTOR GetRotation();
    void SetRotationQuaternion(const DirectX::XMFLOAT4& _vec);
    void XM_CALLCONV SetRotationQuaternion(DirectX::FXMVECTOR _vec);
    void SetRotationYPR(const DirectX::XMFLOAT3& _ypr);
    void XM_CALLCONV SetRotationYPR(DirectX::FXMVECTOR _ypr);
    void RotateYPR(const DirectX::XMFLOAT3& _ypr);

    const DirectX::XMFLOAT3& GetScalingFLOAT() const;
    DirectX::XMVECTOR GetScaling();
    void SetScaling(const DirectX::XMFLOAT3& _vec);
    void XM_CALLCONV SetScaling(DirectX::FXMVECTOR _vec);
    void Scale(const DirectX::XMFLOAT3& _scale);

    const DirectX::XMFLOAT4X4& GetMatrixFLOAT();
    DirectX::XMMATRIX GetMatrix();


    DirectX::XMFLOAT3 Forward();
    DirectX::XMFLOAT3 Right();
    DirectX::XMFLOAT3 Up();



    DirectX::XMMATRIX operator * (TRANSFORM& _other);
    DirectX::XMMATRIX XM_CALLCONV operator * (DirectX::FXMMATRIX _other);
    DirectX::XMVECTOR operator * (const DirectX::XMFLOAT3& _other);

    void Reset();
};

#endif // !TRANSFORM_INCLUDE__H