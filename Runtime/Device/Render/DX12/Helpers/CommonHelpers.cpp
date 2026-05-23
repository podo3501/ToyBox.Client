#include "pch.h"
#include "CommonHelpers.h"

size_t AlignSize(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}