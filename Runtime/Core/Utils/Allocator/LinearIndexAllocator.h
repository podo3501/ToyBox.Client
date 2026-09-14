#pragma once
#include "IndexType.h"
#include <atomic>

namespace Core
{
    class LinearIndexAllocator
    {
    public:
        void Initialize(Index capacity) noexcept;
        Index Allocate(Index count = 1) noexcept;

        void Reset() noexcept; // 전체 리소스를 다시 로드하는 등 특수한 경우에만 사용
        Index Capacity() const noexcept { return m_capacity; }
        Index Allocated() const noexcept { return m_allocated.load(std::memory_order_relaxed); }

    private:
        Index m_capacity{ 0 };
        std::atomic<Index> m_allocated{ 0 };
    };
}
