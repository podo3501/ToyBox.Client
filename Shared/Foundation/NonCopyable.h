#pragma once

class NonCopyable
{
public:
    virtual ~NonCopyable() = default;
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};