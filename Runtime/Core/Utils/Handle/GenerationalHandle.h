#pragma once
#include <cstdint>

template<typename Tag>
class GenerationalHandle
{
public:
    constexpr GenerationalHandle() = default;

    constexpr uint16_t Index() const noexcept { return static_cast<uint16_t>(m_value & 0xFFFF); }
    constexpr uint16_t Generation() const noexcept { return static_cast<uint16_t>((m_value >> 16) & 0xFFFF); }

    constexpr explicit operator bool() const noexcept { return m_value != 0; }

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

    template<typename T, typename Tag2, size_t MaxCount>
    friend class FixedHandlePool;
};
