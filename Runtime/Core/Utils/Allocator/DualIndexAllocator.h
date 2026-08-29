#pragma once
#include "IndexType.h"

namespace Core
{
    class DualIndexAllocator
    {
    public:
        void Initialize(Index capacity) noexcept;
        Index AllocateFront() noexcept;
        Index AllocateBack(Index count) noexcept; // count개의 연속 슬롯 시작 인덱스를 반환 (뒤에서부터 채움)
        
        void ResetBack() noexcept { m_back = 0; }
        void ResetAll() noexcept { m_front = 0; m_back = 0; }

        Index Capacity() const noexcept { return m_capacity; }

    private:
        Index m_capacity = 0;
        Index m_front = 0;
        Index m_back = 0;
    };
}