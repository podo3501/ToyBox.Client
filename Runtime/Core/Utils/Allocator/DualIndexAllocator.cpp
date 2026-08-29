#include "pch.h"
#include "DualIndexAllocator.h"

namespace Core
{
    void DualIndexAllocator::Initialize(Index capacity) noexcept
    {
        m_capacity = capacity;
        m_front = 0;
        m_back = 0;
    }

    Index DualIndexAllocator::AllocateFront() noexcept
    {
        if (m_front + m_back >= m_capacity)
        {
            Assert(false); // 공간이 가득 참. 지워서 만들던지, 늘리던지.
            return InvalidIndex;
        }
        return m_front++;
    }

    Index DualIndexAllocator::AllocateBack(Index count) noexcept // count개의 연속 슬롯 시작 인덱스를 반환 (뒤에서부터 채움)
    {
        Index used = m_back + count;
        if (used > m_capacity - m_front)
        {
            Assert(false);
            return InvalidIndex;
        }
        m_back = used;
        return m_capacity - used;
    }
}