#include "pch.h"
#include "BindlessDescriptorAllocator.h"
#include "BackendConfig.h"
#include "Core/Device.h"

BindlessDescriptorAllocator::~BindlessDescriptorAllocator()
{
    Assert(!m_dynamicRegion.HasOutstanding()); //작업중인 것이 없어야 한다.
}
BindlessDescriptorAllocator::BindlessDescriptorAllocator() = default;

bool BindlessDescriptorAllocator::Initialize(Device& device) noexcept
{
    m_persistentRegion.Initialize(BindlessDescriptors::PersistentCount); // persistent: [0, persistentCount)

    m_dynamicOffset = BindlessDescriptors::PersistentCount;
    m_dynamicRegion.Initialize(BindlessDescriptors::DynamicCount); // dynamic: [persistentCount, persistentCount + dynamicCount)

    m_transientBase = m_dynamicOffset + BindlessDescriptors::DynamicCount;
    m_transientSlotCapacity = BindlessDescriptors::TransientCount; // transient: [transientBase, transientBase + transientSlotCapacity), 슬롯당 config.transientCount

    m_heap = device.CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        BindlessDescriptors::Count,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    if (!m_heap)
        return false;

    m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
    return true;
}

UINT BindlessDescriptorAllocator::AllocatePersistent() noexcept
{
    return m_persistentRegion.Allocate();
}

UINT BindlessDescriptorAllocator::AllocateTransient(uint32_t slot, UINT count) noexcept
{
    Assert(slot < FrameBufferCount);

    UINT local = m_transientOffset[slot].fetch_add(count, std::memory_order_relaxed);
    if (local + count > m_transientSlotCapacity)
    {
        Assert(false); // 슬롯 용량 초과 - config.transientCount를 늘려야 함
        return UINT_MAX;
    }

    UINT slotBase = m_transientBase + m_transientSlotCapacity * slot;
    return slotBase + local;
}

UINT BindlessDescriptorAllocator::AllocateDynamic() noexcept
{
    UINT local = m_dynamicRegion.Allocate();
    if (local == Core::InvalidIndex)
        return UINT_MAX;

    return m_dynamicOffset + local; // 로컬 인덱스를 전역 heap 인덱스로 변환(그냥 앞에 공간 더함)
}

void BindlessDescriptorAllocator::FreeDynamic(UINT index) noexcept
{
    if (index == UINT_MAX) return;
    m_dynamicRegion.Free(index - m_dynamicOffset); // 전역 인덱스를 로컬 인덱스로 변환(그냥 앞에 공간 뺌)
}

void BindlessDescriptorAllocator::ResetTransient(uint32_t slot) noexcept
{
    Assert(slot < FrameBufferCount);
    m_transientOffset[slot] = 0;
}

void BindlessDescriptorAllocator::ResetAll() noexcept
{
    m_persistentRegion.Reset();
    m_dynamicRegion.Reset();
    for (auto& offset : m_transientOffset)
        offset = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE BindlessDescriptorAllocator::GetCpuHandle(UINT index) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE BindlessDescriptorAllocator::GetGpuHandle(UINT index) const noexcept
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}