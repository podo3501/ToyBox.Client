#include "pch.h"
#include "RGRenderIDAllocator.h"

bool RGRenderIDAllocator::Initialize(uint32_t totalCapacity, uint32_t dynamicCapacity) noexcept
{
    if (totalCapacity <= dynamicCapacity) return false;

    m_dynamicStart = totalCapacity - dynamicCapacity; // 로컬 -> 전역 인덱스 변환용 오프셋
    m_persistentRegion.Initialize(m_dynamicStart);    // [0, dynamicStart), front=persistent, back=transient
    m_dynamicRegion.Initialize(dynamicCapacity);      // 로컬 인덱스, free 가능

    return true;
}

RGResourceID RGRenderIDAllocator::AllocatePersistent() noexcept
{
    return m_persistentRegion.AllocateFront();
}

RGResourceID RGRenderIDAllocator::AllocateTransient(uint32_t count) noexcept
{
    return m_persistentRegion.AllocateBack(count);
}

RGResourceID RGRenderIDAllocator::AllocateDynamic() noexcept
{
    Core::Index local = m_dynamicRegion.Allocate();
    if (local == Core::InvalidIndex)
        return InvalidRGID;

    return m_dynamicStart + local; // 전역 인덱스로 변환
}

void RGRenderIDAllocator::FreeDynamic(RGResourceID id) noexcept
{
    m_dynamicRegion.Free(id - m_dynamicStart); // 전역 -> 로컬
}

void RGRenderIDAllocator::ResetTransient() noexcept
{ 
    m_persistentRegion.ResetBack(); 
}

void RGRenderIDAllocator::ResetAll() noexcept
{
    m_persistentRegion.ResetAll(); //transient 부분도 reset 됨.
    m_dynamicRegion.Reset();
}