#include "pch.h"
#include "BindlessDescriptorAllocator.h"
#include "Core/Device.h"

namespace BindlessDescriptors
{
    static constexpr uint32_t MaxCount = 524288; // 1,000,000개가 최대치. 이 값을 넘지 않게 설정.
    static constexpr uint32_t PersistentCount = 393216;  // [0, persistentCount) - 해제 없는 영구 할당 (텍스처/머티리얼 등)
    static constexpr uint32_t DynamicCount = 32768;      // [persistentCount, persistentCount + dynamicCount) - 개별 free 가능, fence 기반
    static constexpr uint32_t TransientCount = 32768;    // 슬롯 하나 크기. 실제 사용량은 TransientCount * FrameBufferCount

    static constexpr uint32_t Count = PersistentCount + DynamicCount + TransientCount * FrameBufferCount;

    static_assert(Count <= MaxCount); //BindlessDescriptorConfig: persistent + dynamic + transient*FrameBufferCount가 최대치를 초과
};

BindlessDescriptorAllocator::~BindlessDescriptorAllocator()
{
    Assert(!m_dynamicRegion.HasOutstanding()); //작업중인 것이 없어야 한다.
}
BindlessDescriptorAllocator::BindlessDescriptorAllocator() = default;

bool BindlessDescriptorAllocator::Initialize(Device& device) noexcept
{
    m_persistentRegion.Initialize(BindlessDescriptors::PersistentCount);

    m_dynamicOffset = BindlessDescriptors::PersistentCount;
    m_dynamicRegion.Initialize(BindlessDescriptors::DynamicCount);

    m_transientOffset = m_dynamicOffset + BindlessDescriptors::DynamicCount;
    for (auto& region : m_transientRegion)
        region.Initialize(BindlessDescriptors::TransientCount); // 슬롯당 capacity는 여기서만 넘겨줌

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

    UINT local = m_transientRegion[slot].Allocate(count);
    if (local == Core::InvalidIndex)
        return UINT_MAX;

    UINT slotBase = m_transientOffset + m_transientRegion[slot].Capacity() * slot;
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
    m_transientRegion[slot].Reset();
}

void BindlessDescriptorAllocator::ResetAll() noexcept
{
    m_persistentRegion.Reset();
    m_dynamicRegion.Reset();
    for (auto& region : m_transientRegion)
        region.Reset();
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