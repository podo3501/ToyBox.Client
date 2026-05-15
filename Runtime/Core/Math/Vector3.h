#pragma once

namespace Core::Math
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector3 operator+(const Vector3& rhs) const;
        Vector3 operator-(const Vector3& rhs) const;
        Vector3 operator*(float scalar) const;
        Vector3 operator/(float scalar) const;

        Vector3& operator+=(const Vector3& rhs);
        Vector3& operator-=(const Vector3& rhs);

        bool operator==(const Vector3& rhs) const;
        bool operator!=(const Vector3& rhs) const;

        float Length() const;
        float LengthSq() const;

        float Dot(const Vector3& rhs) const;
        Vector3 Cross(const Vector3& rhs) const;

        Vector3 Normalized() const;
        void Normalize();

        static Vector3 Zero();
        static Vector3 One();

        static Vector3 Up();
        static Vector3 Right();
        static Vector3 Forward();
    };
}
