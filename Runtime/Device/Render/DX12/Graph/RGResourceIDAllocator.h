#pragma once
#include "Core/Utils/Allocator/DualIndexAllocator.h"
#include "Core/Utils/Allocator/IndexAllocator.h"
#include "RGTypes.h"

static constexpr uint32_t TotalResourceIDCapacity = 250;
static constexpr uint32_t DynamicResourceIDCapacity = 50;

class RGResourceIDAllocator
{
public:
    bool Initialize(uint32_t totalCapacity, uint32_t dynamicCapacity) noexcept;

    RGResourceID AllocatePersistent() noexcept;
    RGResourceID AllocateTransient(uint32_t count = 1) noexcept;
    RGResourceID AllocateDynamic() noexcept;

    void FreeDynamic(RGResourceID id) noexcept;
    void ResetTransient() noexcept;
    void ResetAll() noexcept;

private:
    Core::DualIndexAllocator m_persistentRegion;
    Core::IndexAllocator m_dynamicRegion;
    Core::Index m_dynamicStart{ 0 }; // 로컬 -> 전역 heap(ID) 인덱스 변환용 오프셋
};