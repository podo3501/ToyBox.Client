#pragma once
#include <cstdint>

struct SubmittedFences
{
    uint64_t direct;
    uint64_t copy;
};

struct CompletedFences
{
    uint64_t direct;
    uint64_t copy;
};