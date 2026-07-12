#include "pch.h"
#include "Vector2.h"

namespace Core
{
    Vector2 Vector2::operator+(const Vector2& rhs) const
    {
        return Vector2(x + rhs.x, y + rhs.y);
    }

    Vector2 Vector2::operator-(const Vector2& rhs) const
    {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    Vector2 Vector2::operator*(float s) const
    {
        return Vector2(x * s, y * s);
    }

    Vector2 Vector2::operator/(float s) const
    {
        return Vector2(x / s, y / s);
    }

    Vector2& Vector2::operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    bool Vector2::operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool Vector2::operator!=(const Vector2& rhs) const
    {
        return !(*this == rhs);
    }

    float Vector2::LengthSq() const
    {
        return x * x + y * y;
    }

    float Vector2::Length() const
    {
        return std::sqrt(LengthSq());
    }

    Vector2 Vector2::Normalized() const
    {
        float len = Length();

        if (len <= 0.00001f)
            return Vector2(0.0f, 0.0f);

        return *this / len;
    }

    Vector2 Vector2::NormalizedOr(const Vector2& fallback) const
    {
        float len = Length();

        if (len <= 0.00001f)
            return fallback;

        return *this / len;
    }

    void Vector2::Normalize()
    {
        float len = Length();

        if (len <= 0.00001f)
            return;

        x /= len;
        y /= len;
    }

    void Vector2::NormalizeOr(const Vector2& fallback)
    {
        float len = Length();

        if (len <= 0.00001f)
        {
            *this = fallback;
            return;
        }

        x /= len;
        y /= len;
    }

    float Vector2::Dot(const Vector2& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    float Vector2::Distance(const Vector2& rhs) const
    {
        return (*this - rhs).Length();
    }

    float Vector2::DistanceSq(const Vector2& rhs) const
    {
        return (*this - rhs).LengthSq();
    }

    Vector2 Vector2::Zero()
    {
        return Vector2(0.0f, 0.0f);
    }

    Vector2 Vector2::One()
    {
        return Vector2(1.0f, 1.0f);
    }
}