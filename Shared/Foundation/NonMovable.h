#pragma once

class NonMovable
{
public:
    ~NonMovable() = default;
    NonMovable() = default;

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};