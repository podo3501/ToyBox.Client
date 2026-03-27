#pragma once

struct Point
{
	int x{ 0 };
	int y{ 0 };

    bool operator==(const Point&) const = default;
    bool operator!=(const Point& rhs) const noexcept { return !(*this == rhs); }
};

inline Point ToPoint(int x, int y)
{
	return { x, y };
}