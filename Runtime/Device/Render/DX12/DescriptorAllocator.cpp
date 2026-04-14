#include "pch.h"
#include "DescriptorAllocator.h"
#include "FenceTypes.h"

struct PendingFree
{
    UINT index;
    SubmittedFences required;
};

DescriptorAllocator::~DescriptorAllocator()
{
    int a = 1;
}
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

UINT DescriptorAllocator::Allocate() noexcept
{
    if (!m_freeList.empty())
    {
        UINT index = m_freeList.back();
        m_freeList.pop_back();
        return index;
    }

    if (m_allocated >= m_capacity)
        return UINT_MAX; // 여기선 단순 처리 (나중에 grow or assert)

    return m_allocated++;
}

void DescriptorAllocator::DeferredFree(UINT index, const SubmittedFences& fences)
{
    m_pendingFrees.push_back({ index, fences }); //double free 방지 없음(현재 구조) 나중에 추가할 예정.
}

void DescriptorAllocator::ProcessDeferredFree(const CompletedFences& completed)
{
    size_t write = 0;

    for (size_t read = 0; read < m_pendingFrees.size(); ++read)
    {
        auto& e = m_pendingFrees[read];

        if (completed.direct >= e.required.direct &&
            completed.copy >= e.required.copy)
            m_freeList.push_back(e.index);
        else
            m_pendingFrees[write++] = e; //쓸것은 앞으로 복사하고 안쓸것은 뒤로 보낸후 resize 한다.
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