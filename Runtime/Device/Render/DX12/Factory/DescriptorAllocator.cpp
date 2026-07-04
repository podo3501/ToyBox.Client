#include "pch.h"
#include "DescriptorAllocator.h"
#include "Core/Device.h"

DescriptorAllocator::~DescriptorAllocator() = default;
DescriptorAllocator::DescriptorAllocator() noexcept = default;

bool DescriptorAllocator::Initialize(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT maxCount) noexcept
{
    m_capacity = maxCount;
    m_allocFront = 0;
    m_shaderVisible = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    m_heap = device.CreateDescriptorHeap(
        type,
        maxCount,
        m_shaderVisible
        ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();

    if (m_shaderVisible)
        m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();

    return true;
}

UINT DescriptorAllocator::Allocate() noexcept
{
    if (m_allocFront + m_allocBack + 1 > m_capacity)
    {
        Assert(false); //디스크립터 힙이 가득 참. 지워서 새로 만들던지, 힙을 늘리던지.
        return UINT_MAX;
    }

    UINT index = m_allocFront;
    m_allocFront += 1;

    return index;
}

UINT DescriptorAllocator::AllocateTransient(UINT count) noexcept
{
    std::lock_guard<std::mutex> lock(m_allocBackMutex);

    UINT start = m_capacity - (m_allocBack + count); // 예: capacity가 1000이고 1개 요청했다면 -> m_allocatedTransient는 1이 됨 -> index는 999번 방 반환
    UINT used = m_allocFront + m_allocBack;

    if (used + count > m_capacity) // 두 영역이 충돌했는지 체크
    {
        Assert(false);
        return UINT_MAX;
    }

    m_allocBack += count; //count만큼 방을 확보
    return start;
}

void DescriptorAllocator::Reset() noexcept
{
    m_allocFront = 0;
    ResetTransient();
}

void DescriptorAllocator::ResetTransient() noexcept
{
    std::lock_guard<std::mutex> lock(m_allocBackMutex);
    m_allocBack = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCpuHandle(UINT index) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}

static D3D12_GPU_DESCRIPTOR_HANDLE InvalidGpuHandle()
{
    D3D12_GPU_DESCRIPTOR_HANDLE h{};
    h.ptr = 0;
    return h;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetGpuHandle(UINT index) const noexcept
{
    if (!m_shaderVisible)
    {
        Assert(false); //shader에서 읽지 못하게끔 초기화 돼 있음. dsv거나 rtv 거나..
        return InvalidGpuHandle();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}