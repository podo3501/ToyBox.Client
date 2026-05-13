#include "pch.h"
#include "DescriptorAllocation.h"
#include "DescriptorAllocator.h"

DescriptorAllocation::~DescriptorAllocation() { Release(); }
DescriptorAllocation::DescriptorAllocation(DescriptorAllocator* allocator, UINT index, UINT count) : 
    m_allocator{ allocator }, 
    m_index{ index }, 
    m_count{ count }
{}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& other) noexcept
{
    MoveFrom(other);
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept
{
    if (this != &other)
    {
        Release();
        MoveFrom(other);
    }

    return *this;
}

void DescriptorAllocation::MoveFrom(DescriptorAllocation& other)
{
    m_allocator = other.m_allocator;
    m_index = other.m_index;
    m_count = other.m_count;

    m_fences = other.m_fences;
    m_deferred = other.m_deferred;

    other.m_allocator = nullptr;
    other.m_index = UINT_MAX;
    other.m_count = 0;
    other.m_deferred = false;
}

void DescriptorAllocation::MarkUsed(const QueueFences& fences)
{
    if (!m_deferred)
    {
        m_fences = fences;
        m_deferred = true;
        return;
    }

    m_fences.Merge(fences);
}

void DescriptorAllocation::MarkUsed(CommandType type, uint64_t fence)
{
    QueueFences f{};

    switch (type)
    {
    case CommandType::Direct: f.direct = fence; break;
    case CommandType::Copy: f.copy = fence; break;
    case CommandType::Compute: f.compute = fence; break;
    }

    MarkUsed(f);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetCpuHandle() const
{
    return m_allocator->GetCpuHandle(m_index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetCpuHandle(UINT offset) const
{
    Assert(offset < m_count);

    auto handle = m_allocator->GetCpuHandle(m_index);
    handle.ptr += offset * m_allocator->GetDescriptorSize();

    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetGpuHandle() const
{
    return m_allocator->GetGpuHandle(m_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetGpuHandle(UINT offset) const
{
    Assert(offset < m_count);

    auto handle = m_allocator->GetGpuHandle(m_index);
    handle.ptr += offset * m_allocator->GetDescriptorSize();

    return handle;
}

void DescriptorAllocation::Release()
{
    if (!m_allocator || m_index == UINT_MAX) return;

    if (m_deferred)
        m_allocator->DeferredFree(m_index, m_count, m_fences);
    else
        m_allocator->Free(m_index, m_count); // CPU heap µî

    m_allocator = nullptr;
    m_index = UINT_MAX;
    m_count = 0;
}