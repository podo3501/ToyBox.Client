#include "pch.h"
#include "DescriptorAllocator.h"
#include "FenceTypes.h"

struct PendingFree
{
    UINT index;
    UINT count;

    QueueFences required;
};

struct FreeBlock
{
    UINT index;
    UINT count;
};

DescriptorAllocator::~DescriptorAllocator() = default;
DescriptorAllocator::DescriptorAllocator(ID3D12Device* device) noexcept :
    m_device{ device }
{}

bool DescriptorAllocator::Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT maxCount, bool shaderVisible) noexcept
{
    m_capacity = maxCount;
    m_allocated = 0;
    m_shaderVisible = shaderVisible;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = maxCount;
    desc.Type = type;
    desc.Flags = shaderVisible
        ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap))))
        return false;

    m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(type);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
    if (shaderVisible)
        m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();

    return true;
}

DescriptorAllocation DescriptorAllocator::Allocate(UINT count) noexcept
{
    Assert(count > 0);

    for (size_t i = 0; i < m_freeList.size(); ++i)
    {
        auto& block = m_freeList[i];
        if (block.count < count)
            continue;

        UINT index = block.index;

        block.index += count; 
        block.count -= count; //block을 소비한다.

        if (block.count == 0)
            m_freeList.erase(m_freeList.begin() + i); // 완전히 사용한 경우 제거

        return DescriptorAllocation(this, index, count);
    }

    if (m_allocated + count > m_capacity)
        return {}; // invalid

    UINT index = m_allocated;
    m_allocated += count;

    return DescriptorAllocation(this, index, count);
}

void DescriptorAllocator::DeferredFree(UINT index, UINT count, const QueueFences& fences)
{
    m_pendingFrees.push_back({ index, count, fences }); //double free 방지 없음(현재 구조) 나중에 추가할 예정.
}

void DescriptorAllocator::Free(UINT index, UINT count)
{
    m_freeList.push_back({ index, count });
}

void DescriptorAllocator::ProcessDeferredFree(const QueueFences& completed)
{
    size_t write = 0;

    for (size_t read = 0; read < m_pendingFrees.size(); ++read)
    {
        auto& e = m_pendingFrees[read];

        bool done =
            completed.direct >= e.required.direct &&
            completed.copy >= e.required.copy;

        if (done)
            Free(e.index, e.count);
        else
            m_pendingFrees[write++] = e; //쓸것은 앞으로 복사하고 안쓸것은 뒤로 보낸후 resize 하는 전형적인 알고리즘.
    }

    m_pendingFrees.resize(write);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCpuHandle(UINT index) const noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetGpuHandle(UINT index) const noexcept
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStart;
    handle.ptr += index * m_descriptorSize;
    return handle;
}