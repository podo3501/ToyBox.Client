#pragma once

class NonMovable
{
public:
    virtual ~NonMovable() = default;
    NonMovable() = default;

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};