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

