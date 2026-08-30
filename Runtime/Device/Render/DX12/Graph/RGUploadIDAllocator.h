#pragma once
#include "Core/Utils/Allocator/IndexAllocator.h"
#include "RGTypes.h"

class RGUploadIDAllocator
{
public:
    void Initialize(uint32_t capacity) noexcept { m_region.Initialize(capacity); }
    RGResourceID Allocate() noexcept;
    void Free(RGResourceID id) noexcept { m_region.Free(id); }
    void Reset() noexcept { m_region.Reset(); }

private:
    Core::IndexAllocator m_region;
};
