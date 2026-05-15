#include "pch.h"
#include "Vector3.h"

namespace Core::Math
{
    Vector3 Vector3::operator+(const Vector3& rhs) const
    {
        return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3 Vector3::operator-(const Vector3& rhs) const
    {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3 Vector3::operator*(float s) const
    {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 Vector3::operator/(float s) const
    {
        return Vector3(x / s, y / s, z / s);
    }

    Vector3& Vector3::operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    bool Vector3::operator==(const Vector3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool Vector3::operator!=(const Vector3& rhs) const
    {
        return !(*this == rhs);
    }

    float Vector3::LengthSq() const
    {
        return x * x + y * y + z * z;
    }

    float Vector3::Length() const
    {
        return std::sqrt(LengthSq());
    }

    Vector3 Vector3::Normalized() const
    {
        float len = Length();
        if (len == 0.0f)
            return Vector3(0, 0, 0);

        return *this / len;
    }

    void Vector3::Normalize()
    {
        float len = Length();
        if (len == 0.0f)
            return;

        x /= len;
        y /= len;
        z /= len;
    }

    float Vector3::Dot(const Vector3& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    Vector3 Vector3::Cross(const Vector3& rhs) const
    {
        return Vector3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x);
    }

    Vector3 Vector3::Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
    Vector3 Vector3::One() { return Vector3(1.0f, 1.0f, 1.0f); }
    Vector3 Vector3::Up() { return Vector3(0.0f, 1.0f, 0.0f); }
    Vector3 Vector3::Right() { return Vector3(1.0f, 0.0f, 0.0f); }
    Vector3 Vector3::Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
}