#include "pch.h"
#include "Geometry2D.h"
#include <assert.h>

bool Rect::Contains(float px, float py) const
{
    assert(width >= 0.f && height >= 0.f);

    return px >= x &&
        px < x + width &&
        py >= y &&
        py < y + height;
}

bool Rect::Contains(const Point& p) const
{
    return Contains(
        static_cast<float>(p.x),
        static_cast<float>(p.y));
}

bool Rect::Contains(const Rect& other) const
{
    return other.x >= x &&
        other.y >= y &&
        other.x + other.width <= x + width &&
        other.y + other.height <= y + height;
}

void Rect::Normalize()
{
    if (width < 0.f)
    {
        x += width;  // x를 왼쪽 끝으로 옮김
        width = -width;  // width를 양수로
    }

    if (height < 0.f)
    {
        y += height;  // y를 위쪽 끝으로 옮김
        height = -height;  // height를 양수로
    }
}

bool Rect::Intersects(const Rect& other) const
{
    return !(other.Left() >= Right() ||
        other.Right() <= Left() ||
        other.Top() >= Bottom() ||
        other.Bottom() <= Top());
}

Rect Rect::Intersect(const Rect& other) const //결과가 음수가 나올 수 있는데, 결과음수 처리하면 연산이 들어가기 때문에 음수가 나가는걸 허용.
{ 
    float l1 = x;
    float t1 = y;
    float r1 = x + width;
    float b1 = y + height;

    float l2 = other.x;
    float t2 = other.y;
    float r2 = other.x + other.width;
    float b2 = other.y + other.height;

    float l = max(l1, l2);
    float t = max(t1, t2);
    float r = min(r1, r2);
    float b = min(b1, b2);

    return { l, t, r - l, b - t }; 
}