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
        slot.handle = Handle{ index, slot.generation };
        slot.data.emplace(std::forward<Args>(args)...);
        slot.alive = true;

        return slot.handle; // generation은 1 이상 보장됨
    }

    bool Remove(Handle h)
    {
        if (!IsValid(h)) return false;

        auto& slot = m_slots[h.index];
        slot.data.reset();
        slot.handle = Handle::Invalid();
        slot.alive = false;

        slot.generation++;
        if (slot.generation == 0) slot.generation = 1; // generation 증가 (0 방지)

        m_freeList.push_back(h.index);
        return true;
    }

    void Clear()
    {
        for (uint32_t i = 1; i < m_slots.size(); ++i) // 0은 dummy
        {
            auto& slot = m_slots[i];

            if (!slot.alive) continue;

            slot.data.reset();
            slot.handle = Handle::Invalid();
            slot.alive = false;

            slot.generation++;
            if (slot.generation == 0) slot.generation = 1;
        }

        m_freeList.clear();
        for (uint32_t i = 1; i < m_slots.size(); ++i)
        {
            m_freeList.push_back(i);
        }
    }

    size_t Size() const
    {
        return m_slots.size();
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

    bool IsValid(Handle h) const
    {
        if (!h) return false;
        if (h.index >= m_slots.size()) return false;

        const auto& slot = m_slots[h.index];
        return slot.alive && slot.generation == h.generation;
    }

    template<typename Func>
    void Visit(Func&& f)
    {
        for (auto& slot : m_slots)
        {
            if (!slot.alive) continue;
            f(slot.handle, *slot.data);
        }
    }

    template<typename Func>
    void Visit(Func&& f) const
    {
        for (const auto& slot : m_slots)
        {
            if (!slot.alive) continue;
            f(slot.handle, *slot.data);
        }
    }

private:
    template<typename T>
    struct HandleSlot
    {
        std::optional<T> data;
        Handle handle{ Handle::Invalid() };
        uint32_t generation = 1; // 0은 사용 안 함
        bool alive = false;
    };

    std::vector<HandleSlot<T>> m_slots;
    std::vector<uint32_t> m_freeList;
};