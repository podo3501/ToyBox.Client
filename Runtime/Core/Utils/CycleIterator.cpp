#include "pch.h"
#include "CycleIterator.h"
#include "assert.h"

//?!? 0 부터 시작하면 문제 없는데 1부터 시작하면 어떻게 될지 모름. 추후에 확인후 수정해야 함.
CycleIterator::CycleIterator() noexcept
    : start(0), end(0), current(0)
{
}

CycleIterator::CycleIterator(int s, int e) noexcept
{
    SetRange(s, e);
}

void CycleIterator::SetRange(int s, int e) noexcept
{
    assert(s < e);

    start = s;
    end = e;
    current = s;
}

static int mod(int value, int range)
{
    return (value % range + range) % range;
}

int CycleIterator::GetCurrent() const noexcept
{
    return current;
}

int CycleIterator::Increase() noexcept
{
    assert(start < end);

    current = start + mod(current + 1 - start, end - start);
    return current;
}

int CycleIterator::Decrease() noexcept
{
    assert(start < end);

    current = start + mod(current - 1 - start, end - start);
    return current;
}

void CycleIterator::Reset() noexcept
{
    current = start;
}