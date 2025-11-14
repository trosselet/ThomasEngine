#ifndef VECTOR_INCLUDE__H
#define VECTOR_INCLUDE__H


#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace DirectX;

// ==============================
// Generic Class
// ==============================
template<typename T, size_t N>
class Vector
{
public:
    T data[N]{};

    // Constructors
    Vector() { for (size_t i = 0; i < N; i++) data[i] = T(0); }
    explicit Vector(T value) { for (size_t i = 0; i < N; i++) data[i] = value; }
    Vector(std::initializer_list<T> list)
    {
        assert(list.size() == N && "Initializer size must match dimension");
        size_t i = 0;
        for (auto v : list) data[i++] = v;
    }

    Vector(T x, T y, T z) requires (N == 3)
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    Vector(const DirectX::XMFLOAT3& v) requires (N == 3)
    {
        data[0] = v.x;
        data[1] = v.y;
        data[2] = v.z;
    }

    // DirectXMath conversion
    operator XMVECTOR() const
    {
        if constexpr (N == 2) return XMLoadFloat2(reinterpret_cast<const XMFLOAT2*>(this));
        if constexpr (N == 3) return XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(this));
        if constexpr (N == 4) return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
        else return XMVectorZero();
    }

    Vector(FXMVECTOR v)
    {
        if constexpr (N == 2) XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(this), v);
        if constexpr (N == 3) XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(this), v);
        if constexpr (N == 4) XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
    }



    // ==============================
    // Conversion helpers
    // ==============================
    [[nodiscard]] DirectX::XMFLOAT2 ToXMFLOAT2() const
    {
        assert(N == 2, "ToXMFLOAT2 is only valid for 2D vectors.");
        return DirectX::XMFLOAT2(data[0], data[1]);
    }

    [[nodiscard]] DirectX::XMFLOAT3 ToXMFLOAT3() const
    {
        assert(N == 3, "ToXMFLOAT3 is only valid for 3D vectors.");
        return DirectX::XMFLOAT3(data[0], data[1], data[2]);
    }

    [[nodiscard]] DirectX::XMFLOAT4 ToXMFLOAT4() const
    {
        assert(N == 4, "ToXMFLOAT4 is only valid for 4D vectors.");
        return DirectX::XMFLOAT4(data[0], data[1], data[2], data[3]);
    }

    Vector(Vector<float, 3> const& other) {
        data[0] = other.data[0];
        data[1] = other.data[1];
        data[2] = other.data[2];
    }

    // ==============================
    // Mathematical Function
    // ==============================
    [[nodiscard]] T Norm() const { return std::sqrt(NormSquared()); }
    [[nodiscard]] T NormSquared() const
    {
        T sum = 0;
        for (size_t i = 0; i < N; i++) sum += data[i] * data[i];
        return sum;
    }

    [[nodiscard]] T Dot(Vector const& other) const
    {
        T sum = 0;
        for (size_t i = 0; i < N; i++) sum += data[i] * other.data[i];
        return sum;
    }

    [[nodiscard]] T DistanceTo(Vector const& other) const
    {
        return (*this - other).Norm();
    }

    [[nodiscard]] T DistanceToSquared(Vector const& other) const
    {
        return (*this - other).NormSquared();
    }

    [[nodiscard]] Vector Normalize() const
    {
        T n = Norm();
        if (n == T(0)) return *this;
        return *this / n;
    }

    [[nodiscard]] Vector Clamp(Vector const& min, Vector const& max) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = std::clamp(data[i], min.data[i], max.data[i]);
        return out;
    }

    [[nodiscard]] Vector Clamp(T min, T max) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = std::clamp(data[i], min, max);
        return out;
    }

    Vector& SelfNormalize()
    {
        T n = Norm();
        if (n != T(0)) *this /= n;
        return *this;
    }

    Vector& SelfClamp(Vector const& min, Vector const& max)
    {
        for (size_t i = 0; i < N; i++) data[i] = std::clamp(data[i], min.data[i], max.data[i]);
        return *this;
    }

    Vector& SelfClamp(T min, T max)
    {
        for (size_t i = 0; i < N; i++) data[i] = std::clamp(data[i], min, max);
        return *this;
    }

    // ==============================
    // Operator
    // ==============================
    [[nodiscard]] Vector operator+(Vector const& other) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] + other.data[i];
        return out;
    }

    [[nodiscard]] Vector operator-(Vector const& other) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] - other.data[i];
        return out;
    }

    [[nodiscard]] Vector operator*(Vector const& other) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] * other.data[i];
        return out;
    }

    [[nodiscard]] Vector operator/(Vector const& other) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] / other.data[i];
        return out;
    }

    [[nodiscard]] Vector operator*(T scalar) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] * scalar;
        return out;
    }

    [[nodiscard]] Vector operator/(T scalar) const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = data[i] / scalar;
        return out;
    }

    [[nodiscard]] Vector operator-() const
    {
        Vector out;
        for (size_t i = 0; i < N; i++) out.data[i] = -data[i];
        return out;
    }

    Vector& operator+=(Vector const& other)
    {
        for (size_t i = 0; i < N; i++) data[i] += other.data[i];
        return *this;
    }

    Vector& operator-=(Vector const& other)
    {
        for (size_t i = 0; i < N; i++) data[i] -= other.data[i];
        return *this;
    }

    Vector& operator*=(Vector const& other)
    {
        for (size_t i = 0; i < N; i++) data[i] *= other.data[i];
        return *this;
    }

    Vector& operator/=(Vector const& other)
    {
        for (size_t i = 0; i < N; i++) data[i] /= other.data[i];
        return *this;
    }

    Vector& operator*=(T scalar)
    {
        for (size_t i = 0; i < N; i++) data[i] *= scalar;
        return *this;
    }

    Vector& operator/=(T scalar)
    {
        for (size_t i = 0; i < N; i++) data[i] /= scalar;
        return *this;
    }

    // ==============================
    // Compare
    // ==============================
    [[nodiscard]] explicit operator bool() const
    {
        for (size_t i = 0; i < N; i++) if (data[i] != T(0)) return true;
        return false;
    }

    [[nodiscard]] bool operator==(Vector const& other) const
    {
        for (size_t i = 0; i < N; i++) if (data[i] != other.data[i]) return false;
        return true;
    }

    [[nodiscard]] bool operator!=(Vector const& other) const
    {
        return !(*this == other);
    }

    [[nodiscard]] bool operator<=(Vector const& other) const
    {
        for (size_t i = 0; i < N; i++) if (data[i] > other.data[i]) return false;
        return true;
    }

    [[nodiscard]] bool operator>=(Vector const& other) const
    {
        for (size_t i = 0; i < N; i++) if (data[i] < other.data[i]) return false;
        return true;
    }

    // Access
    T& operator[](size_t i) { return data[i]; }
    T const& operator[](size_t i) const { return data[i]; }
};

#endif // !VECTOR_INCLUDE__H