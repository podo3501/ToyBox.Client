#pragma once
#include "GenerationalHandle.h"
#include <array>
#include <vector>
#include <type_traits>

template<typename T, typename Tag, size_t MaxCount>
class FixedHandlePool
{
public:
    using Handle = GenerationalHandle<Tag>;

    FixedHandlePool() = default;

    void Setup(uint16_t maxCount)
    {
        if (maxCount > MaxCount) // 최대 usable 제한
            maxCount = MaxCount;
        m_capacity = maxCount + 1; // 내부 capacity = usable + dummy

        m_freeList.clear();
        for (uint16_t i = 1; i < m_capacity; ++i) 
            m_freeList.push_back(i); // index 1 ~ maxCount 사용 가능
    }

    template<typename... Args>
    Handle Emplace(Args&&... args)
    {
        if (m_freeList.empty())
            return Handle::Invalid();

        uint16_t index = m_freeList.back();
        m_freeList.pop_back();

        auto& slot = m_slots[index];
        slot.handle = Handle::Make(index, slot.generation);
        slot.data = T(std::forward<Args>(args)...);
        slot.alive = true;

        return slot.handle;
    }

    bool Remove(Handle h)
    {
        if (!IsValid(h)) return false;

        auto& slot = m_slots[h.Index()];
        slot.handle = Handle::Invalid();
        slot.alive = false;

        ++slot.generation;
        if (slot.generation == 0) slot.generation = 1; // generation 증가 (0 방지)

        m_freeList.push_back(h.Index());
        return true;
    }

    void Clear()
    {
        for (uint16_t i = 1; i < m_capacity; ++i)
        {
            auto& slot = m_slots[i];

            if (slot.alive)
            {
                slot.data = T{};
                slot.alive = false;
                slot.handle = Handle::Invalid();

                ++slot.generation;
                if (slot.generation == 0) slot.generation = 1;
            }
        }

        m_freeList.clear();
        for (uint16_t i = 1; i < m_capacity; ++i)
            m_freeList.push_back(i);
    }

    T* Get(Handle h)
    {
        if (!IsValid(h)) return nullptr;
        return &m_slots[h.Index()].data;
    }

    const T* Get(Handle h) const
    {
        if (!IsValid(h)) return nullptr;
        return &m_slots[h.Index()].data;
    }

    bool IsValid(Handle h) const
    {
        if (!h) return false;

        uint16_t idx = h.Index();
        if (idx == 0 || idx >= m_capacity) return false;

        const auto& slot = m_slots[idx];
        return slot.alive && slot.generation == h.Generation();
    }

    template<typename Func>
    void Visit(Func&& f)
    {
        for (auto& slot : m_slots)
        {
            if (!slot.alive) continue;
            f(slot.handle, slot.data);
        }
    }

    template<typename Func>
    void Visit(Func&& f) const
    {
        for (const auto& slot : m_slots)
        {
            if (!slot.alive) continue;
            f(slot.handle, slot.data);
        }
    }

private:
    struct Slot
    {
        T data{};
        Handle handle{ Handle::Invalid() };
        uint16_t generation = 1;
        bool alive = false;
    };

    std::array<Slot, MaxCount + 1> m_slots{}; // +1 해서 index 0 dummy 확보
    std::vector<uint16_t> m_freeList;
    uint16_t m_capacity{ 1 }; // 항상 최소 1 (dummy만 있는 상태)
};