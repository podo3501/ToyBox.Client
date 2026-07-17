#include "pch.h"
#include "BindlessDescriptorAllocator.h"
#include "Core/Device.h"

BindlessDescriptorAllocator::~BindlessDescriptorAllocator() = default;
BindlessDescriptorAllocator::BindlessDescriptorAllocator() = default;

bool BindlessDescriptorAllocator::Initialize(Device& device, const BindlessDescriptorConfig& config) noexcept
{
    Assert(config.bindlessCount > config.asyncTransientCount);

    m_allocFront = 0;
    m_frameAllocBack = 0;
    m_asyncTransientStart = config.bindlessCount - config.asyncTransientCount;
    m_asyncTransientAlloc = m_asyncTransientStart;
    m_asyncTransientCount = config.asyncTransientCount;

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
    if (m_allocFront + m_frameAllocBack >= m_asyncTransientStart)
    {
        Assert(false); //디스크립터 힙이 가득 참. 지워서 새로 만들던지, 힙을 늘리던지.
        return UINT_MAX;
    }

    return m_allocFront++;
}

UINT BindlessDescriptorAllocator::AllocateFrameTransient(UINT count) noexcept
{
    UINT used = m_frameAllocBack + count;
    if (used > m_asyncTransientStart - m_allocFront)
    {
        Assert(false);
        return UINT_MAX;
    }

    UINT start = m_asyncTransientStart - used;
    m_frameAllocBack += count; //count만큼 방을 확보
    return start;
}

UINT BindlessDescriptorAllocator::AllocateAsyncTransient(UINT count) noexcept
{
    std::lock_guard<std::mutex> lock(m_asyncMutex);

    UINT end = m_asyncTransientStart + m_asyncTransientCount;
    if (m_asyncTransientAlloc + count > end)
    {
        Assert(false); // 할당한 공간이 다 찼다.
        return UINT_MAX;
    }

    UINT index = m_asyncTransientAlloc;
    m_asyncTransientAlloc += count;
    return index;
}

void BindlessDescriptorAllocator::ResetAll() noexcept
{
    m_allocFront = 0;
    ResetFrameTransient();
    ResetAsyncTransient();
}

void BindlessDescriptorAllocator::ResetFrameTransient() noexcept
{
    m_frameAllocBack = 0;
}

void BindlessDescriptorAllocator::ResetAsyncTransient() noexcept
{
    //할당한 부분들을 지우게끔 수정해야 하지만, 일단은 지금 mipmap 한군데 밖에 쓰지 않기 때문에 이렇게
    //처리하지만, 추후에 이걸 쓰게 되면 이 함수는 보강해야 한다.
    std::lock_guard<std::mutex> lock(m_asyncMutex);
    m_asyncTransientAlloc = m_asyncTransientStart;
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