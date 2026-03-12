#pragma once

class CycleIterator
{
public:
    CycleIterator() noexcept;
    CycleIterator(int s, int e) noexcept;
    void SetRange(int s, int e) noexcept;

    int Increase() noexcept;
    int Decrease() noexcept;
    int GetCurrent() const noexcept;
    void Reset() noexcept;

private:
    int current{}, start{}, end{};
};
