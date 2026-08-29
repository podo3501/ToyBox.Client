#include "pch.h"
#include "BindlessDescriptorAllocator.h"
#include "Core/Device.h"

BindlessDescriptorAllocator::~BindlessDescriptorAllocator() = default;
BindlessDescriptorAllocator::BindlessDescriptorAllocator() = default;

bool BindlessDescriptorAllocator::Initialize(Device& device, const BindlessDescriptorConfig& config) noexcept
{
    Assert(config.bindlessCount > config.asyncTransientCount);

    m_asyncTransientStart = config.bindlessCount - config.asyncTransientCount;
    m_persistentRegion.Initialize(m_asyncTransientStart);
    m_asyncRegion.Initialize(config.asyncTransientCount);

    m_heap = device.CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        config.bindlessCount,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    if (!m_heap)
        return false;

    m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
    return true;
}

UINT BindlessDescriptorAllocator::Allocate() noexcept
{
    return m_persistentRegion.AllocateFront();
}

UINT BindlessDescriptorAllocator::AllocateFrameTransient(UINT count) noexcept
{
    return m_persistentRegion.AllocateBack(count);
}

UINT BindlessDescriptorAllocator::AllocateAsyncTransient() noexcept
{
    UINT local = m_asyncRegion.Allocate();
    if (local == Core::InvalidIndex)
        return UINT_MAX;

    return m_asyncTransientStart + local; // 로컬 인덱스를 전역 heap 인덱스로 변환(그냥 앞에 공간 더함)
}

void BindlessDescriptorAllocator::FreeAsyncTransient(UINT index) noexcept
{
    if (index == UINT_MAX) return;
    m_asyncRegion.Free(index - m_asyncTransientStart); // 전역 인덱스를 로컬 인덱스로 변환(그냥 앞에 공간 뺌)
}

void BindlessDescriptorAllocator::ResetAll() noexcept
{
    m_persistentRegion.ResetAll();
    ResetAsyncTransient();
}

void BindlessDescriptorAllocator::ResetFrameTransient() noexcept
{
    m_persistentRegion.ResetBack();
}

void BindlessDescriptorAllocator::ResetAsyncTransient() noexcept
{
    m_asyncRegion.Reset();
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