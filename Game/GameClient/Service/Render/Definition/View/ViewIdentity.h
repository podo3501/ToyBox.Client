#pragma once
#include <cstdint>
#include <functional>

class ViewIdentity
{
public:
    constexpr ViewIdentity() noexcept = default;
    constexpr ViewIdentity(uint32_t value) noexcept : m_value{ value } {}

    static constexpr ViewIdentity Invalid() noexcept { return ViewIdentity{}; }
    constexpr bool IsValid() const noexcept { return m_value != kInvalid; }
    constexpr uint32_t Value() const noexcept { return m_value; }

    constexpr bool operator==(const ViewIdentity&) const noexcept = default;

private:
    static constexpr uint32_t kInvalid{ 0 };
    uint32_t m_value{ kInvalid }; //지금은 uint지만 나중에는 enum 값으로 바뀔수도 있다.
};

struct ViewIdentityHash
{
    size_t operator()(const ViewIdentity& id) const noexcept
    {
        return std::hash<uint64_t>{}(id.Value());
    }
};
