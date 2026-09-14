#include "pch.h"
#include "LinearIndexAllocator.h"

namespace Core
{
    void LinearIndexAllocator::Initialize(Index capacity) noexcept
    {
        m_capacity = capacity;
        m_allocated = 0;
    }

    Index LinearIndexAllocator::Allocate(Index count) noexcept
    {
        Index index = m_allocated.fetch_add(count, std::memory_order_relaxed);
        if (index + count > m_capacity)
        {
            Assert(false); // 공간이 가득 참. capacity를 늘려야 함.
            return InvalidIndex;
        }
        return index;
    }

    void LinearIndexAllocator::Reset() noexcept
    {
        m_allocated = 0;
    }
}