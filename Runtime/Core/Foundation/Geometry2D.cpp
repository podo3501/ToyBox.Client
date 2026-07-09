#include "pch.h"
#include "Geometry2D.h"
#include <assert.h>

bool Rect::Contains(int px, int py) const
{
    assert(width >= 0 && height >= 0);

    return px >= x &&
        px < x + width &&
        py >= y &&
        py < y + height;
}

bool Rect::Contains(float px, float py) const
{
    assert(width >= 0 && height >= 0);

    return px >= x &&
        px < x + width &&
        py >= y &&
        py < y + height;
}

bool Rect::Contains(const Point& p) const
{
    return Contains(p.x, p.y);
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
    if (width < 0)
    {
        x += width;  // x를 왼쪽 끝으로 옮김
        width = -width;  // width를 양수로
    }

    if (height < 0)
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
    int l1 = x, t1 = y, r1 = x + width, b1 = y + height;
    int l2 = other.x, t2 = other.y, r2 = other.x + other.width, b2 = other.y + other.height;

    int l = max(l1, l2);
    int t = max(t1, t2);
    int r = min(r1, r2);
    int b = min(b1, b2);

    return { l, t, r - l, b - t }; 
}