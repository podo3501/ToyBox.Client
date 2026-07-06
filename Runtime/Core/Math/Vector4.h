#pragma once
#include <cmath>

namespace Core::Math
{
    struct Vector3;

    struct Vector4
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        Vector4() = default;
        Vector4(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
        Vector4(const Vector3& v3, float inW);

        Vector4 operator+(const Vector4& rhs) const;
        Vector4 operator-(const Vector4& rhs) const;
        Vector4 operator*(float s) const;
        Vector4 operator/(float s) const;

        Vector4& operator+=(const Vector4& rhs);
        Vector4& operator-=(const Vector4& rhs);

        bool operator==(const Vector4& rhs) const;
        bool operator!=(const Vector4& rhs) const;

        float LengthSq() const;
        float Length() const;

        Vector4 Normalized() const;
        Vector4 NormalizedOr(const Vector4& fallback) const;
        void Normalize();
        void NormalizeOr(const Vector4& fallback);

        float Dot(const Vector4& rhs) const;

        static Vector4 Zero();
        static Vector4 One();
    };
}
