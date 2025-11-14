#ifndef MATRIX_4X4_INCLUDE__H
#define MATRIX_4X4_INCLUDE__H

#include <DirectXMath.h>
#include <cstring>
#include <cassert>
#include <initializer_list>

using namespace DirectX;

class Matrix4x4
{
public:
    float m[4][4]{};

    // Identity
    void SetIdentity()
    {
        std::memset(m, 0, sizeof(m));
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    }

    // Constructors
    Matrix4x4()
    {
        SetIdentity();
    }

    explicit Matrix4x4(float diag)
    {
        SetIdentity();
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = diag;
    }

    Matrix4x4(std::initializer_list<float> values)
    {
        assert(values.size() == 16 && "Matrix4x4 needs 16 values");
        std::memcpy(m, values.begin(), sizeof(float) * 16);
    }

    // Conversion DirectXMath
    operator XMMATRIX() const { return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this)); }
    Matrix4x4(FXMMATRIX mat) { XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), mat); }

    // Transpose
    Matrix4x4 Transpose() const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = m[j][i];
        return result;
    }

    // Matrix mul
    Matrix4x4 operator*(const Matrix4x4& rhs) const
    {
        Matrix4x4 result{};
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                result.m[i][j] =
                    m[i][0] * rhs.m[0][j] +
                    m[i][1] * rhs.m[1][j] +
                    m[i][2] * rhs.m[2][j] +
                    m[i][3] * rhs.m[3][j];
            }
        }
        return result;
    }


    static Matrix4x4 PerspectiveFovLH(float fov, float aspect, float nearZ, float farZ)
    {
        XMMATRIX xm = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
        return Matrix4x4(xm);
    }

    static Matrix4x4 LookAtLH(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up)
    {
        XMMATRIX xm = XMMatrixLookAtLH(eye, target, up);
        return Matrix4x4(xm);
    }
};

#endif // !MATRIX_4X4_INCLUDE__H
