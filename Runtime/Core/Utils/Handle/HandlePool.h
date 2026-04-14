#pragma once
#include "IDHandle.h"
#include <vector>

template<typename T, typename Tag>
class HandlePool
{
public:
    using Handle = IDHandle<Tag>;

    HandlePool()
    {
        m_slots.emplace_back(); // index 0은 dummy를 하나 넣어서 절대 0 인덱스가 생성되지 않게 한다. 아니면 인덱스에 계산식이 들어가야 한다.
    }

    template<typename... Args>
    Handle Emplace(Args&&... args)
    {
        uint32_t index;

        if (!m_freeList.empty())
        {
            index = m_freeList.back();
            m_freeList.pop_back();
        }
        else
        {
            index = static_cast<uint32_t>(m_slots.size());
            m_slots.emplace_back();
        }

        auto& slot = m_slots[index];
        slot.data.reset();
        slot.data.emplace(std::forward<Args>(args)...);
        slot.alive = true;

        return Handle{ index, slot.generation }; // generation은 1 이상 보장됨
    }

    T* Find(Handle h)
    {
        if (!IsValid(h)) return nullptr;
        return &(*m_slots[h.index].data);
    }

    const T* Find(Handle h) const
    {
        if (!IsValid(h)) return nullptr;
        return &(*m_slots[h.index].data);
    }

    bool Remove(Handle h)
    {
        if (!IsValid(h)) return false;

        auto& slot = m_slots[h.index];
        slot.data.reset();
        slot.alive = false;

        slot.generation++;
        if (slot.generation == 0) slot.generation = 1; // generation 증가 (0 방지)

        m_freeList.push_back(h.index);
        return true;
    }

    bool IsValid(Handle h) const
    {
        if (!h) return false;
        if (h.index >= m_slots.size()) return false;

        const auto& slot = m_slots[h.index];
        return slot.alive && slot.generation == h.generation;
    }

private:
    template<typename T>
    struct HandleSlot
    {
        std::optional<T> data;
        uint32_t generation = 1; // 0은 사용 안 함
        bool alive = false;
    };

    std::vector<HandleSlot<T>> m_slots;
    std::vector<uint32_t> m_freeList;
};