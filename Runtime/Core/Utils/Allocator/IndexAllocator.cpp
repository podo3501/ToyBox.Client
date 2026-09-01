#include "pch.h"
#include "IndexAllocator.h"

namespace Core
{
    void IndexAllocator::Initialize(Index capacity) noexcept
    {
        m_capacity = capacity;
        m_allocated = 0;
        m_freeList.clear();
    }

    Index IndexAllocator::Allocate() noexcept
    {
        std::lock_guard lock(m_mutex);
        if (!m_freeList.empty())
        {
            uint32_t index = m_freeList.back();
            m_freeList.pop_back();
            return index;
        }
        if (m_allocated >= m_capacity)
        {
            Assert(false); // 가득 참. 지워서 새로 만들던지, 늘리던지.
            return InvalidIndex;
        }
        return m_allocated++;
    }

    void IndexAllocator::Free(Index  index) noexcept
    {
        if (index == InvalidIndex) return;

        std::lock_guard lock(m_mutex);
        Assert(index < m_allocated); // 중복 해제 방지용 최소 안전장치
        m_freeList.push_back(index);
    }

    void IndexAllocator::Reset() noexcept
    {
        std::lock_guard lock(m_mutex);

        m_allocated = 0;
        m_freeList.clear();
    }

    bool IndexAllocator::HasOutstanding() noexcept
    {
        std::lock_guard lock(m_mutex);
        return m_allocated - static_cast<Index>(m_freeList.size()) > 0;
    }
}