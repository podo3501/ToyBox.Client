#include "pch.h"
#include "DX12Utils.h"
#include "d3dx12.h"

size_t AlignSize(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}