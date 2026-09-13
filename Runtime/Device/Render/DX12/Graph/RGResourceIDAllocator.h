#pragma once
#include "Core/Utils/Allocator/LinearIndexAllocator.h"
#include "Core/Utils/Allocator/IndexAllocator.h"
#include "RGTypes.h"

static constexpr uint32_t PersistentResourceIDCapacity = 150;
static constexpr uint32_t DynamicResourceIDCapacity = 50;
static constexpr uint32_t TransientResourceIDCapacity = 100;
static constexpr uint32_t TotalResourceIDCapacity =
    PersistentResourceIDCapacity + DynamicResourceIDCapacity + TransientResourceIDCapacity;

class RGResourceIDAllocator
{
public:
    ~RGResourceIDAllocator();
    RGResourceIDAllocator() noexcept;

    RGResourceID AllocatePersistent() noexcept;
    RGResourceID AllocateDynamic() noexcept;
    RGResourceID AllocateTransient() noexcept;

    void FreeDynamic(RGResourceID id) noexcept;
    void ResetTransient() noexcept;
    void ResetAll() noexcept;

private:
    Core::LinearIndexAllocator m_persistentRegion; // [0, persistentCount), 해제 없음
    Core::IndexAllocator m_dynamicRegion;          // [dynamicOffset, dynamicOffset + dynamicCount), 해제 가능
    uint32_t m_dynamicOffset{ 0 };

    Core::LinearIndexAllocator m_transientRegion;  // [transientOffset, transientOffset + transientCount), 그래프 빌드마다 리셋
    uint32_t m_transientOffset{ 0 };
};