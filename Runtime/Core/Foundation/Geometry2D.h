#pragma once

struct Point
{
	int x{ 0 };
	int y{ 0 };

    bool operator==(const Point&) const = default;
    bool operator!=(const Point& rhs) const noexcept { return !(*this == rhs); }
};

inline Point ToPoint(int x, int y) { return { x, y }; }

struct Size
{
	uint32_t width{ 0 };
	uint32_t height{ 0 };

	bool operator==(const Size&) const = default;
	bool operator!=(const Size& rhs) const noexcept { return !(*this == rhs); }
};

inline Size ToSize(uint32_t w, uint32_t h) { return { w, h }; }

struct Rect
{
    float x{ 0.f }, y{ 0.f }, width{ 0.f }, height{ 0.f };

    bool IsValid() const { return width > 0.f && height > 0.f; }

    float Left() const { return x; }
    float Right() const { return x + width; }
    float Top() const { return y; }
    float Bottom() const { return y + height; }

    bool Contains(float px, float py) const;
    bool Contains(const Point& p) const;
    bool Contains(const Rect& other) const;

    void Normalize(); //뒤집힌 사각형( width, height가 음수인 경우) 일 경우에 정상적인 사각형으로 만들어줌.

    bool Intersects(const Rect& other) const;
    Rect Intersect(const Rect& other) const;
};
