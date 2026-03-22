#pragma once
#include <cstdint>
#include <array>
#include <vector>

template<typename Tag>
class GenerationalHandle
{
public:
    constexpr GenerationalHandle() = default;

    constexpr uint16_t Index() const noexcept { return static_cast<uint16_t>(m_value & 0xFFFF); }
    constexpr uint16_t Generation() const noexcept { return static_cast<uint16_t>((m_value >> 16) & 0xFFFF); }

    constexpr explicit operator bool() const noexcept { return m_value != 0; }
    constexpr bool IsValid() const noexcept { return m_value != 0; }
    static constexpr GenerationalHandle Invalid() noexcept { return GenerationalHandle{ 0 }; }

    bool operator==(const GenerationalHandle&) const = default;
    bool operator!=(const GenerationalHandle&) const = default;

private:
    constexpr explicit GenerationalHandle(uint32_t v) : m_value(v) {}
    static constexpr GenerationalHandle Make(uint16_t index, uint16_t gen)
    {
        return GenerationalHandle{
            static_cast<uint32_t>(index) |
            (static_cast<uint32_t>(gen) << 16)
        };
    }

    uint32_t m_value{ 0 };

    template<typename T, size_t N>
    friend class HandleAllocator;
};

//generation을 써서 handle의 유효성을 검증한다. 슬롯이 정해져 있다면, 재사용을 해야 하는데 그때 handle이 유효한지 판단해야 하기 때문이다.
//슬롯이 정해져 있기 때문에 vector나 array같은 것들과 함께 쓰이며 이때 접근이 빠르고 캐쉬가 좋기 때문에 이 handle과 같이 사용한다.
template<typename Tag, size_t MaxCount>
class HandleAllocator
{
public:
    using H = GenerationalHandle<Tag>;

    explicit HandleAllocator(uint16_t userMax) : 
        m_userMaxVoices(userMax)
    {
        if (m_userMaxVoices > MaxCount)
            m_userMaxVoices = MaxCount; // 절대 상한
    }

    H Acquire()
    {
        uint16_t index;

        if (!m_freeList.empty())
        {
            index = m_freeList.back();
            m_freeList.pop_back();
        }
        else
        {
            if (m_size >= m_userMaxVoices)
                return H::Invalid();

            index = m_size++; //m_size는 한번이라도 사용한 슬롯의 위치이다. 왜 이걸 기억하고 있냐면, 이게 만약 없다면 freeslot에도 없으면 남은 슬롯에서 for를 돌면서 찾아야 하기 때문이다. 이게 있어서 O(1)이 만족한다.
        }

        Slot& slot = m_slots[index];
        slot.alive = true;

        return H::Make(index, slot.generation);
    }

    void Release(H h)
    {
        if (!IsValid(h)) return;

        Slot& slot = m_slots[h.Index()];
        slot.alive = false;

        ++slot.generation;
        if (slot.generation == 0)
            slot.generation = 1;

        m_freeList.push_back(h.Index());
    }

    bool IsValid(H h) const
    {
        if (!h) return false;

        uint16_t idx = h.Index();
        if (idx >= m_size) return false;

        const Slot& slot = m_slots[idx];
        return slot.alive && slot.generation == h.Generation();
    }

private:
    struct Slot
    {
        uint16_t generation = 1;
        bool alive = false;
    };

    std::array<Slot, MaxCount> m_slots{};
    std::vector<uint16_t> m_freeList; //재사용 슬롯을 모아놓은 구조. 여기에 없으면 m_size를 하나 올린다.
    uint16_t m_userMaxVoices{ 0 };     // 유저 설정 최대 활성 voice
    uint16_t m_size{ 0 }; //열린 슬롯의 위치.
};