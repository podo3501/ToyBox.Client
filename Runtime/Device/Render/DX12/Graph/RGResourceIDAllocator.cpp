#include "pch.h"
#include "RGResourceIDAllocator.h"

RGResourceIDAllocator::~RGResourceIDAllocator() = default;
RGResourceIDAllocator::RGResourceIDAllocator() noexcept
{
    m_persistentRegion.Initialize(PersistentResourceIDCapacity); // persistent: [0, PersistentResourceIDCapacity)

    m_dynamicOffset = PersistentResourceIDCapacity;
    m_dynamicRegion.Initialize(DynamicResourceIDCapacity); // dynamic: [dynamicOffset, dynamicOffset + DynamicResourceIDCapacity)

    m_transientOffset = m_dynamicOffset + DynamicResourceIDCapacity;
    m_transientRegion.Initialize(TransientResourceIDCapacity); // transient: [transientOffset, transientOffset + TransientResourceIDCapacity)
}

RGResourceID RGResourceIDAllocator::AllocatePersistent() noexcept
{
    return m_persistentRegion.Allocate();
}

RGResourceID RGResourceIDAllocator::AllocateTransient() noexcept
{
    RGResourceID local = m_transientRegion.Allocate();
    if (local == Core::InvalidIndex)
        return InvalidRGID;

    return m_transientOffset + local;
}

RGResourceID RGResourceIDAllocator::AllocateDynamic() noexcept
{
    Core::Index local = m_dynamicRegion.Allocate();
    if (local == Core::InvalidIndex)
        return InvalidRGID;

    return m_dynamicOffset + local; // 전역 인덱스로 변환
}

void RGResourceIDAllocator::FreeDynamic(RGResourceID id) noexcept
{
    if (id == InvalidRGID) return;
    m_dynamicRegion.Free(id - m_dynamicOffset); // 전역 -> 로컬
}

void RGResourceIDAllocator::ResetTransient() noexcept
{ 
    m_transientRegion.Reset();
}

void RGResourceIDAllocator::ResetAll() noexcept
{
    m_persistentRegion.Reset();
    m_dynamicRegion.Reset();
    m_transientRegion.Reset();
}