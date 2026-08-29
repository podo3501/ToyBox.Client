#pragma once
#include "IndexType.h"

namespace Core
{
    class IndexAllocator
    {
    public:
        void Initialize(Index capacity) noexcept;
        Index Allocate() noexcept;
        void Free(Index  index) noexcept;
        void Reset() noexcept;

        Index Capacity() const noexcept { return m_capacity; }

    private:
        Index m_capacity{ 0 };
        Index m_allocated{ 0 };
        std::vector<Index> m_freeList;
        std::mutex m_mutex;
    };
}
