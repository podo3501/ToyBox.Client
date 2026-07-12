#include "pch.h"
#include "Vector4.h"
#include "Vector3.h"

namespace Core
{
     Vector4::Vector4(const Vector3& v3, float inW) : 
         x(v3.x), y(v3.y), z(v3.z), w(inW) 
     {}

    Vector4 Vector4::operator+(const Vector4& rhs) const
    {
        return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Vector4 Vector4::operator-(const Vector4& rhs) const
    {
        return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vector4 Vector4::operator*(float s) const
    {
        return Vector4(x * s, y * s, z * s, w * s);
    }

    Vector4 Vector4::operator/(float s) const
    {
        return Vector4(x / s, y / s, z / s, w / s);
    }

    Vector4& Vector4::operator+=(const Vector4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    bool Vector4::operator==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool Vector4::operator!=(const Vector4& rhs) const
    {
        return !(*this == rhs);
    }

    float Vector4::LengthSq() const
    {
        return x * x + y * y + z * z + w * w;
    }

    float Vector4::Length() const
    {
        return std::sqrt(LengthSq());
    }

    Vector4 Vector4::Normalized() const
    {
        float len = Length();

        if (len <= 0.00001f)
            return Vector4(0.0f, 0.0f, 0.0f, 0.0f);

        return *this / len;
    }

    Vector4 Vector4::NormalizedOr(const Vector4& fallback) const
    {
        float len = Length();

        if (len <= 0.00001f)
            return fallback;

        return *this / len;
    }

    void Vector4::Normalize()
    {
        float len = Length();
        if (len <= 0.00001f)
            return;

        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }

    void Vector4::NormalizeOr(const Vector4& fallback)
    {
        float len = Length();

        if (len <= 0.00001f)
        {
            *this = fallback;
            return;
        }

        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }

    float Vector4::Dot(const Vector4& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }

    Vector4 Vector4::Zero() { return Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
    Vector4 Vector4::One() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
}