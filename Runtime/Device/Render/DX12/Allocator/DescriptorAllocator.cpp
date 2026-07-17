#include "pch.h"
#include "DescriptorAllocator.h"
#include "Core/Device.h"

DescriptorAllocator::~DescriptorAllocator() = default;
DescriptorAllocator::DescriptorAllocator() = default;

bool DescriptorAllocator::Initialize(
    Device& device,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    UINT capacity) noexcept
{
    Assert(type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    m_capacity = capacity;
    m_allocated = 0;

    m_heap = device.CreateDescriptorHeap(
        type,
        capacity,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    if (!m_heap)
        return false;

    m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();

    return true;
}

UINT DescriptorAllocator::Allocate() noexcept
{
    if (m_allocated >= m_capacity)
    {
        Assert(false); //디스크립터 힙이 가득 참. 지워서 새로 만들던지, 힙을 늘리던지.
        return UINT_MAX;
    }

    return m_allocated++;
}

void DescriptorAllocator::Reset() noexcept
{
    // GPU idle 이후, 전체 descriptor를 재사용하기 위한 reset
    m_allocated = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCpuHandle(UINT index) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}