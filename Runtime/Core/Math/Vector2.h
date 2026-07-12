#pragma once

namespace Core
{
    struct Vector2
    {
        float x;
        float y;

        Vector2() : x(0), y(0) {}
        Vector2(float x, float y) : x(x), y(y) {}

        Vector2 operator+(const Vector2& rhs) const;
        Vector2 operator-(const Vector2& rhs) const;
        Vector2 operator*(float scalar) const;
        Vector2 operator/(float scalar) const;

        Vector2& operator+=(const Vector2& rhs);
        Vector2& operator-=(const Vector2& rhs);

        bool operator==(const Vector2& rhs) const;
        bool operator!=(const Vector2& rhs) const;

        float Length() const;
        float LengthSq() const;

        float Dot(const Vector2& rhs) const;
        float Distance(const Vector2& rhs) const;
        float DistanceSq(const Vector2& rhs) const;

        Vector2 Normalized() const;
        Vector2 NormalizedOr(const Vector2& fallback) const;

        void Normalize();
        void NormalizeOr(const Vector2& fallback);

        static Vector2 Zero();
        static Vector2 One();
    };
}
