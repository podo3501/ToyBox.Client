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

    m_indexAllocator.Initialize(capacity);
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

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCpuHandle(UINT index) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}